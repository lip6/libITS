// Copyright (C) 2009, 2010, 2016 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "dsog.hh"
#include <string>
#include <cassert>
#include <spot/misc/hashfunc.hh>
#include <spot/twa/bddprint.hh>

//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

using namespace spot;


namespace dsog
{

  dsog_div_state::dsog_div_state(const state* left_state, const bdd& c) :
    left_state_(left_state),
    cond(c)
  {
  }

  dsog_div_state::~dsog_div_state()
  {
    left_state_->destroy();
  }

  dsog_div_state::dsog_div_state(const dsog_div_state& c)
  {
    assert(0);
  }

  dsog_div_state&
  dsog_div_state::operator=(const dsog_div_state& c)
  {
    assert(0);
    return *this;
  }

  int dsog_div_state::compare(const state* other) const {
    if (!dynamic_cast<const dsog_div_state*>(other))
      return -1;
    const dsog_div_state* m = dynamic_cast<const dsog_div_state*>(other);
    assert(m);
    int n = m->get_left_state()->compare(left_state_);
    if (n != 0)
      return n;
    return cond.id() - m->cond.id();
  }

  size_t dsog_div_state::hash() const {
    return wang32_hash(cond.id());
  }

  spot::state* dsog_div_state::clone() const {
    return new dsog_div_state(left_state_, cond);
  }

  const bdd& dsog_div_state::get_condition() const {
    return cond;
  }

  std::ostream & dsog_div_state::print (std::ostream & os) const {
    return (os << "DSOGDivState " << std::endl);
  }

  ////////////////////////////////////////////////////////////
  // state_product
  dsog_state::dsog_state(const dsog_state& o)
    : state(),
      left_(o.left()->clone()),
      right_(o.right()),
      div_(o.div_),
      succ_(o.succ_),
      cond_(o.cond_)
  {
  }

  dsog_state::dsog_state(const spot::state* left,
			 const sogIts & model, its::State right, bdd bddAP)
    :	left_(left), cond_(bddAP)
  {
    right_ = model.leastPostTestFixpoint(right, bddAP);
    div_ = (model.getDivergent(right_, bddAP) != its::State::null);
    succ_ = ((!model.getSelector(bddAP)) & model.getNextRel()) (right_);
  }

  dsog_state::~dsog_state()
  {
    left_->destroy();
  }

  int
  dsog_state::compare(const state* other) const
  {
    if (dynamic_cast<const dsog_div_state*>(other))
      return 1;
    const dsog_state* o = dynamic_cast<const dsog_state*>(other);
    assert(o);
    int res = left_->compare(o->left());
    if (res != 0)
      return res;
    if (right_ == o->right())
      return 0;
    else
      return right_ < o->right() ? 1 : -1;
  }

  size_t
  dsog_state::hash() const
  {
    // We assume that size_t is 32-bit wide.
    return wang32_hash(left_->hash()) ^ right_.hash();
  }

  dsog_state*
  dsog_state::clone() const
  {
    return new dsog_state(*this);
  }

  ////////////////////////////////////////////////////////////
  // dsog_succ_iterator

    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
  dsog_succ_iterator::dsog_succ_iterator(const dsog_tgba* aut,
					 const dsog_state* s,
					 spot::twa_succ_iterator* left_iter,
					 const sogIts & model)
    : has_div(s->get_div()),
      cur(s),			// ITS state
      aut_(aut),
      left_(s->left()),		// TGBA state
      left_iter_(left_iter),
      model_(model),
      right_(s->right()),
      dest_(0),
      succstates_(s->get_succ()),
      itap(0)
  {
  }

  dsog_succ_iterator::~dsog_succ_iterator()
  {
    delete left_iter_;
    delete itap;
  }

  void
  dsog_succ_iterator::step()
  {
    if (has_div) // divergence
      {
	trace << "finding next diverging transition" << std::endl;

	bdd cond = cur->get_cond();
	while ((!left_iter_->done())
	       && ((left_iter_->cond() & cond) != cond))
	  left_iter_->next();

	if (!left_iter_->done())
	  return;

	trace << "divergence done" << std::endl;

	has_div = false;

	if (succstates_ == its::State::null)
	  {
	    // left_iter_ is already done().
	    return;
	  }

	left_iter_->first();
      }

    trace << "find next regular transition" << std::endl;

    // regular transitions
    do
      {
	if (itap != 0)
	  {
	    if (!itap->done())
	      itap->next();
	    if (itap->done())
	      {
		left_iter_->next();
		delete itap;
		itap = 0;
		if (left_iter_->done())
		  return;
	      }
	  }
	if (itap == 0)
	  {
	    bdd cond = cur->get_cond();
	    while ((left_iter_->cond() & cond) != cond)
	      {
		left_iter_->next();
		if (left_iter_->done())
		  return;
	      }

	    bdd ap = aut_->ap_reachable_from_left(left_iter_->dst());
	    sogits::APIterator::varset_t vars;

	    // Convert ap into a vector of variable numbers.
	    while (ap != bddtrue)
	      {
		vars.push_back(bdd_var(ap));
		ap = bdd_high(ap);
	      }

	    delete itap;
	    itap = sogits::APIteratorFactory::create_new(vars);

	    /// position "it" at first of ap bdd set
	    itap->first();

	    // Nothing to split.
	    if (vars.empty())
	      {
		assert(itap->done());

		delete dest_;
		dest_ = new dsog_state(left_iter_->dst(), model_,
				       succstates_, bddtrue);
		assert(dest_->right() != its::State::null);
		return;
	      }
	  }

	// iterate until a non empty succ is found (or end reached)
	for (; !itap->done(); itap->next())
	  {
	    delete dest_;
	    dest_ = new dsog_state(left_iter_->dst(), model_,
				   succstates_, itap->current());
	    if (dest_->right() != its::State::null)
	      return;
	  }
      }
    while (!left_iter_->done());
  }


  bool
  dsog_succ_iterator::first()
  {
    left_iter_->first();
    if (left_iter_->done())
      return false;

    if ((!has_div) &&
	(succstates_ == its::State::null))
      {
	while (!left_iter_->done())
	  left_iter_->next();
	return false;
      }

    step();
    return !done();
  }

  bool
  dsog_succ_iterator::next()
  {
    if (has_div)
      left_iter_->next();
    // otherwise we must advance itap, and step() always does it.
    step();
    return !done();
  }


  bool
  dsog_succ_iterator::done() const
  {
    return left_iter_->done();
  }


  spot::state*
  dsog_succ_iterator::dst() const
  {
    if (has_div)
      {
	bdd ap = aut_->ap_reachable_from_left(left_iter_->dst());
	return new dsog_div_state(left_iter_->dst(),
				  bdd_existcomp(cur->get_cond(), ap));
      }
    else
      return dest_->clone();
  }

  bdd
  dsog_succ_iterator::cond() const
  {
    assert(!done());
    return left_iter_->cond() & cur->get_cond();
  }

  spot::acc_cond::mark_t dsog_succ_iterator::acc() const
  {
    assert(!done());
    return left_iter_->acc();
  }


  dsog_div_succ_iterator::dsog_div_succ_iterator(const dsog_tgba* aut,
						 const bdd& c,
						 twa_succ_iterator* li)
    : aut_(aut), cond_(c), left_iter_(li)
  {
  }


  void dsog_div_succ_iterator::step()
  {
    while ((!left_iter_->done()) &&
	   ((left_iter_->cond() & cond_) != cond_))
      left_iter_->next();
  }

  bool dsog_div_succ_iterator::first() {
    left_iter_->first();
    step();
    return !done();
  }

  bool dsog_div_succ_iterator::next() {
    left_iter_->next();
    step();
    return !done();
  }

  bool dsog_div_succ_iterator::done() const {
    return  left_iter_->done();
  }

  spot::state* dsog_div_succ_iterator::dst() const {
    assert(!done());
    trace << "FIRING : " << format_transition() << std::endl;
    trace << "FROM a div state" << std::endl << std::endl;

    bdd ap = aut_->ap_reachable_from_left(left_iter_->dst());
    return new dsog_div_state(left_iter_->dst(),
			      bdd_existcomp(cond_, ap));
  }

  bdd dsog_div_succ_iterator::cond() const {
    assert(!done());
    return cond_;
  }

  spot::acc_cond::mark_t dsog_div_succ_iterator::acc() const {
    assert(!done());
    return left_iter_->acc();
  }

  std::string dsog_div_succ_iterator::format_transition() const {
    assert(!done());
    std::ostringstream os;
    spot::bdd_print_formula(os, aut_->get_dict(), cond_);
    return "div(" + os.str() + ")";
  }


  ////////////////////////////////////////////////////////////
  // dsog_tgba

  /// \brief Constructor.
  /// \param left The left automata in the product.
  /// \param right The ITS model.
  dsog_tgba::dsog_tgba(const spot::const_twa_graph_ptr& left,
		       const sogIts & right)
    : spot::twa(left->get_dict()), left_(left), model_(right), scc_(left)
  {
    // we use the same APs as the left automaton.
    copy_ap_of(left);
    // Compute the set of APs reachable from each SCC.
    unsigned n = scc_.scc_count();
    scc_ap_.reserve(n);
    // SCCs are visited in a topological order.  An SCC x can only
    // have a successor SCC y such that y < x.
    for (unsigned s = 0; s < n; ++s)
      {
	bdd aps = bddtrue;
        // Collect the aps on the transitions leaving states in this
        // SCC.
	for (auto st: scc_.states_of(s))
	  for (auto& t: left->out(st))
	    aps &= bdd_support(t.cond);
        // Add aps in the successor SCCs.
        for (auto succ: scc_.succ(s))
          aps &= scc_ap_[succ];
        scc_ap_.push_back(aps);
      }
  }

  dsog_tgba::~dsog_tgba()
  {
  }

  bdd
  dsog_tgba::ap_reachable_from_left(const state* s) const
  {
    return scc_ap_[scc_.scc_of(left_->state_number(s))];
  }

  state*
  dsog_tgba::get_init_state() const
  {
    const spot::state* lis = left_->get_init_state();
    bdd ap = ap_reachable_from_left(lis);

    trace << ap << std::endl;

    sogits::APIterator::varset_t vars;

    // Convert ap into a vector of variable numbers.
    while (ap != bddtrue)
      {
	vars.push_back(bdd_var(ap));
	ap = bdd_high(ap);
      }

    its::State m0 = model_.getInitialState() ;
    assert(m0 != its::State::null);

    // now determine which AP are true in m0
    sogits::APIterator* it = sogits::APIteratorFactory::create_new(vars);
    for (it->first(); !it->done(); it->next() ) {
      its::Transition selector = model_.getSelector(it->current());
      its::State msel = selector(m0);
      trace << "testing " << it->current() << std::endl;
      if (msel != its::State::null) {
	dsog_state *init = new dsog_state(lis, model_, m0, it->current());
	trace << "Initial state of tgba :" << format_state(init)
	      << "verifies :"<< it->current() << std::endl;
	delete it;

	return init;
      }
    }

    // No AP expected.
    if (vars.empty())
      {
	dsog_state *init = new dsog_state(lis, model_, m0, bddtrue);
	assert(init->right() != its::State::null);
	return init;
      }

    assert(!"no conjunction of AP is verified by m0 ???");
    // for compiler happiness
    delete it;
    return 0;
  }

  spot::twa_succ_iterator*
  dsog_tgba::succ_iter(const state* local_state) const
  {
    const dsog_div_state* d = dynamic_cast<const dsog_div_state*>(local_state);
    if (d)
      {
	twa_succ_iterator* li = left_->succ_iter(d->get_left_state());
	return new dsog_div_succ_iterator(this, d->get_condition(), li);
      }

    const dsog_state* s =
      dynamic_cast<const dsog_state*>(local_state);
    assert(s);

    twa_succ_iterator* li = left_->succ_iter(s->left());
    return new dsog_succ_iterator(this, s, li, model_);
  }

  std::string
  dsog_tgba::format_state(const state* state) const
  {
    const dsog_state* s = dynamic_cast<const dsog_state*>(state);
    if (s)
      {
	std::stringstream ss;
	if (s->right().nbStates() <= 15)
	  model_.getType()->printState(s->right(), ss) ;

	return (left_->format_state(s->left())
		+ " * "
		+ " SDD size: " + to_string(s->right().nbStates()) + " hash:" + to_string(s->right().hash())
		+ (s->get_div() ? " (div)" : "")
		+ ss.str());
      }
    else
      {
	const dsog_div_state* d = dynamic_cast<const dsog_div_state*>(state);
	assert(d);

	std::ostringstream os;
	os << left_->format_state(d->get_left_state()) << " * div_state(";
	spot::bdd_print_formula(os, get_dict(), d->get_condition()) << ")";

	return os.str();
      }
  }

  state*
  dsog_tgba::project_state(const state* s, const spot::const_twa_ptr& t) const
  {
    const dsog_state* s2 = dynamic_cast<const dsog_state*>(s);
    assert(s2);
    if (t.get() == this)
      return s2->clone();
    state* res = left_->project_state(s2->left(), t);
    if (res)
      return res;

    // TODO : to allow right projection, we need to encapsulate
    // an aggregate of the ITS states inside a (dedicated?) spot::state
    return 0;
  }

}
