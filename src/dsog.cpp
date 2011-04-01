// Copyright (C) 2009, 2010 Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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
#include "misc/hashfunc.hh"
#include "tgba/bddprint.hh"

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
    const dsog_div_state* m = dynamic_cast<const dsog_div_state*>(other);
    if (!dynamic_cast<const dsog_div_state*>(other))
      return -1;
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
    delete left_;
  }

  int
  dsog_state::compare(const state* other) const
  {
    const dsog_state* o = dynamic_cast<const dsog_state*>(other);
    if (dynamic_cast<const dsog_div_state*>(other))
      return 1;
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
					 spot::tgba_succ_iterator* left_iter,
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
	while ((!left_iter_->done()) &&
	       ((left_iter_->current_condition() & cond) != cond))
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
	    while ((left_iter_->current_condition() & cond) != cond)
	      {
		left_iter_->next();
		if (left_iter_->done())
		  return;
	      }

	    const spot::scc_map& scc = aut_->get_scc_map();
	    unsigned sccn = scc.scc_of_state(left_iter_->current_state());
	    bdd ap =        scc.aprec_set_of(sccn);

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
		dest_ = new dsog_state(left_iter_->current_state(), model_,
				       succstates_, bddtrue);
		assert(dest_->right() != its::State::null);
		return;
	      }
	  }

	// iterate until a non empty succ is found (or end reached)
	for (; !itap->done(); itap->next())
	  {
	    delete dest_;
	    dest_ = new dsog_state(left_iter_->current_state(), model_,
				   succstates_, itap->current());
	    if (dest_->right() != its::State::null)
	      return;
	  }
      }
    while (!left_iter_->done());
  }


  void
  dsog_succ_iterator::first()
  {
    left_iter_->first();
    if (left_iter_->done())
      return;

    if ((!has_div) &&
	(succstates_ == its::State::null))
      {
	while (!left_iter_->done())
	  left_iter_->next();
	return;
      }

    step();
  }

  void
  dsog_succ_iterator::next()
  {
    if (has_div)
      left_iter_->next();
    // otherwise we must advance itap, and step() always does it.
    step();
  }


  bool
  dsog_succ_iterator::done() const
  {
    return left_iter_->done();
  }


  spot::state*
  dsog_succ_iterator::current_state() const
  {
    if (has_div)
      {
	// AP(left_iter_->current_state()).
	const spot::scc_map& scc = aut_->get_scc_map();
	unsigned sccn = scc.scc_of_state(left_iter_->current_state());
	bdd ap =        scc.aprec_set_of(sccn);

	return new dsog_div_state(left_iter_->current_state(),
				  bdd_existcomp(cur->get_cond(), ap));
      }
    else
      return dest_->clone();
  }

  bdd
  dsog_succ_iterator::current_condition() const
  {
    assert(!done());
    return left_iter_->current_condition() & cur->get_cond();
  }

  bdd dsog_succ_iterator::current_acceptance_conditions() const
  {
    assert(!done());
    return left_iter_->current_acceptance_conditions();
  }


  dsog_div_succ_iterator::dsog_div_succ_iterator(const dsog_tgba* aut,
						 const bdd& c,
						 tgba_succ_iterator* li)
    : aut_(aut), cond(c), left_iter_(li)
  {
  }


  void dsog_div_succ_iterator::step()
  {
    while ((!left_iter_->done()) &&
	   ((left_iter_->current_condition() & cond) != cond))
      left_iter_->next();
  }

  void dsog_div_succ_iterator::first() {
    left_iter_->first();
    step();
  }

  void dsog_div_succ_iterator::next() {
    left_iter_->next();
    step();
  }

  bool dsog_div_succ_iterator::done() const {
    return  left_iter_->done();
  }

  spot::state* dsog_div_succ_iterator::current_state() const {
    assert(!done());
    trace << "FIRING : " << format_transition() << std::endl;
    trace << "FROM a div state" << std::endl << std::endl;

    // AP(left_iter_->current_state()).
    const spot::scc_map& scc = aut_->get_scc_map();
    unsigned sccn = scc.scc_of_state(left_iter_->current_state());
    bdd ap =        scc.aprec_set_of(sccn);

    return new dsog_div_state(left_iter_->current_state(),
			      bdd_existcomp(cond, ap));
  }

  bdd dsog_div_succ_iterator::current_condition() const {
    assert(!done());
    return cond;
  }

  bdd dsog_div_succ_iterator::current_acceptance_conditions() const {
    assert(!done());
    return left_iter_->current_acceptance_conditions();
  }

  std::string dsog_div_succ_iterator::format_transition() const {
    assert(!done());
    std::ostringstream os;
    spot::bdd_print_formula(os, aut_->get_dict(), cond);
    return "div(" + os.str() + ")";
  }

  dsog_div_succ_iterator::dsog_div_succ_iterator(const dsog_div_succ_iterator& s) {
    assert(false);
  }

  dsog_div_succ_iterator& dsog_div_succ_iterator::operator=(const dsog_div_succ_iterator& s) {
    assert(false);
    return *this;
  }



  ////////////////////////////////////////////////////////////
  // dsog_tgba

  /// \brief Constructor.
  /// \param left The left automata in the product.
  /// \param right The ITS model.
  dsog_tgba::dsog_tgba(const spot::tgba* left, const sogIts & right)
    : dict_(left->get_dict()), left_(left), model_(right), scc_(left)
  {
    // register that we use the same bdd variables (dict) as the automaton.
    // these vars are unregistered in dtor
    dict_->register_all_variables_of(&left_, this);
    // Compute the map of SCC, with extra informations.  We are
    // particularly interested in the set of APs that are reachable
    // from a given SCC.
    scc_.build_map();
  }

  dsog_tgba::~dsog_tgba()
  {
    dict_->unregister_all_my_variables(this);
  }

  const spot::scc_map&
  dsog_tgba::get_scc_map() const
  {
    return scc_;
  }

  state*
  dsog_tgba::get_init_state() const
  {
    bdd bddAP;
    const state* lis = left_->get_init_state();
    unsigned sccn = scc_.scc_of_state(lis);
    bdd ap =        scc_.aprec_set_of(sccn);

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

  spot::tgba_succ_iterator*
  dsog_tgba::succ_iter(const state* local_state,
		       const state* global_state,
		       const tgba* global_automaton) const
  {
    const dsog_div_state* d = dynamic_cast<const dsog_div_state*>(local_state);
    if (d)
      {
	tgba_succ_iterator* li = left_->succ_iter(d->get_left_state(),
						  global_state,
						  global_automaton);

	return new dsog_div_succ_iterator(this, d->get_condition(), li);
      }

    const dsog_state* s =
      dynamic_cast<const dsog_state*>(local_state);
    assert(s);

    tgba_succ_iterator* li = left_->succ_iter(s->left(),
					      global_state,
					      global_automaton);

    return new dsog_succ_iterator(this, s, li, model_);
  }

  bdd
  dsog_tgba::compute_support_conditions(const state* in) const
  {
    const dsog_state* s = dynamic_cast<const dsog_state*>(in);
    assert(s);
    return left_->support_conditions(s->left());
  }

  bdd
  dsog_tgba::compute_support_variables(const state* in) const
  {
    const dsog_state* s = dynamic_cast<const dsog_state*>(in);
    assert(s);
    return left_->support_variables(s->left());
  }

  bdd_dict*
  dsog_tgba::get_dict() const
  {
    return dict_;
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
	spot::bdd_print_formula(os, dict_, d->get_condition()) << ")";

	return os.str();
      }
  }

  state*
  dsog_tgba::project_state(const state* s, const tgba* t) const
  {
    const dsog_state* s2 = dynamic_cast<const dsog_state*>(s);
    assert(s2);
    if (t == this)
      return s2->clone();
    state* res = left_->project_state(s2->left(), t);
    if (res)
      return res;

    // TODO : to allow right projection, we need to encapsulate
    // an aggregate of the ITS states inside a (dedicated?) spot::state
    return 0;
  }

  bdd
  dsog_tgba::all_acceptance_conditions() const
  {
    return left_->all_acceptance_conditions();
  }

  bdd
  dsog_tgba::neg_acceptance_conditions() const
  {
    return left_->neg_acceptance_conditions();
  }

  std::string
  dsog_tgba::transition_annotation(const tgba_succ_iterator* t) const
  {
    const dsog_succ_iterator* i =
      dynamic_cast<const dsog_succ_iterator*>(t);
    assert(i);
    return left_->transition_annotation(i->left_iter_);
  }

}
