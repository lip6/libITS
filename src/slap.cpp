// Copyright (C) 2003, 2004, 2006, 2009, 2016 Laboratoire
// d'Informatique de Paris 6 (LIP6), d�partement Syst�mes R�partis
// Coop�ratifs (SRC), Universit� Pierre et Marie Curie.
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

#include "slap.hh"
#include <string>
#include <cassert>
#include <spot/misc/hashfunc.hh>
#include <spot/twa/bddprint.hh>
#include "tgbaIts.hh"

// for fsltl type optimization of terminal states
#include "fsltl.hh"

using namespace spot;

// #define trace std::cerr
#define trace while (0) std::cerr


namespace slap
{

  ////////////////////////////////////////////////////////////
  // state_product
  slap_state::slap_state(const slap_state& o)
    : state(),
      left_(o.left()->clone()),
      right_(o.right())
  {
  }

  slap_state::~slap_state()
  {
    left_->destroy();
  }

  int
  slap_state::compare(const state* other) const
  {
    if (dynamic_cast<const slap_div_state*>(other))
      return 1;

    const slap_state* o = dynamic_cast<const slap_state*>(other);
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
  slap_state::hash() const
  {
    // We assume that size_t is 32-bit wide.
    return wang32_hash(left_->hash()) ^ right_.hash();
  }

  slap_state*
  slap_state::clone() const
  {
    return new slap_state(*this);
  }

  ////////////////////////////////////////////////////////////
  // slap_succ_iterator

    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
  slap_succ_iterator::slap_succ_iterator(const twa* aut,
					 const spot::state * aut_state,
					 twa_succ_iterator* left,
					 const sogIts & model,
					 const its::State& right,
					 sogits::FSTYPE fsType)
    : aut_(aut),
      aut_state_(aut_state),
      left_(left),
      model_(model),
      right_(right),
      fsType_(fsType)
  {

    /** Test whether there is a self loop labeled with ALL acceptance conditions */
//     for (left_->first() ; ! left_->done() ; left_->next() ) {
//       if ( left_->current_state()->compare(aut_state) == 0 ) {
// //	std::cerr << "on arc :" << aut_->transition_annotation(left_) << std::endl;
//	if ( left_->current_acceptance_conditions() ==  aut_->all_acceptance_conditions()  ) {
//	  trace << "\ndetected condition !" << std::endl;
//	}
//       }
//     }

  }

  slap_succ_iterator::~slap_succ_iterator()
  {
    delete left_;
  }

  bdd
  slap_succ_iterator::compute_weaker_selfloop_ap()
  {
    // The acceptance condition labeling the arc of the tgba
    acc_cond::mark_t ac = left_->acc();
    // The state reached in the tgba through this arc
    const state * q2 = left_->dst();

    // Iterate over q2's successors, and add to F the atomic props which are on arcs "weaker" (wrt ac)
    bdd F = bddfalse;

    for (auto it: aut_->succ(q2))
      {
	const state * dest = it->dst();
	// Test self loop
	if ( dest->compare(q2) == 0 ) {
	  // Test ac=>ac' (subsume the arc)
	  acc_cond::mark_t acprime = it->acc();
	  if ( (ac & acprime) == acprime)
	    F |= it->cond();
      }
      dest->destroy();
    }
    q2->destroy();

    return F;
  }

  void
  slap_succ_iterator::step_()
  {
    // Test if this TGBA arc is a self-loop without acceptance conditions, if the TGBA has acceptance conds
    if (aut_->num_sets() > 0)
      {
	if (left_->acc() == 0U) {
	  const state * q2 = left_->dst();
	  if (q2->compare(aut_state_) == 0)
	    dest_ = its::State::null;
	  q2->destroy();
	}
    }


    // progress to "entry" states of succ in ITS model
    bdd curcond = left_->cond();
    dest_ = model_.succSatisfying ( right_, curcond );

    // test if we have empty initial states => we can avoid computing subsumed arcs etc...
    if (dest_.empty())
      return;

    // grab the current subsumed arc conditions
    bdd F = compute_weaker_selfloop_ap();

    // costly saturate the aggregate
    dest_ = model_.leastPreTestFixpoint (dest_, F);

  }

  void
  slap_succ_iterator::next_non_false_()
  {
    while (!left_->done())
      {
	step_();

	if (! dest_.empty() ) {
	  if ( fsType_ == sogits::FSA
	       && left_->dst()->compare(aut_state_) == 0
	       && dest_ - right_ == its::State::null ) {
	    left_->next();
	    std::cerr << "Subset !!"<< std::endl;
	    continue;
	  }
	  // reached an appropriate non empty successor
	  return ;
	}
	// (else)
	// progress in tgba
	left_->next();
      }
  }

  bool
  slap_succ_iterator::first()
  {
    left_->first();
    next_non_false_();
    return !done();
  }

  bool
  slap_succ_iterator::next()
  {
    // Progress in the tgba succs
    left_->next();
    // search for a non empty succ aggregate
    next_non_false_();
    return !done();
  }

  bool
  slap_succ_iterator::done() const
  {
    return left_->done();
  }


  const slap_state*
  slap_succ_iterator::dst() const
  {
    return new slap_state(left_->dst(), dest_);
  }

  bdd
  slap_succ_iterator::cond() const
  {
    return left_->cond();
  }

  spot::acc_cond::mark_t slap_succ_iterator::acc() const
  {
    return left_->acc();
  }

  ////////////////////////////////////////////////////////////
  // slap_tgba

  /// \brief Constructor.
  /// \param left The left automata in the product.
  /// \param right The ITS model.
  slap_tgba::slap_tgba(spot::const_twa_ptr left, const sogIts & right,sogits::FSTYPE fsType)
    : spot::twa(left->get_dict()), left_(left), model_(right), fsType_(fsType)
  {
    // We use the same atomic propositions as left.
    copy_ap_of(left);
    copy_acceptance_of(left);
  }

  slap_tgba::~slap_tgba()
  {
  }

  state*
  slap_tgba::get_init_state() const
  {

    // The initial state of the tgba
    const state* init_tgba = left_->get_init_state();
    // Iterate over init states successors, and add to F the atomic props which are on arcs without acceptance conds
    bdd F = bddfalse;

    for (auto it: left_->succ(init_tgba))
      if ( isSelfLoop(it, init_tgba) ) {
	// Test ac=0 (empty acceptance cond arcs)
	auto ac = it->acc();
	if (ac == 0U && left_->num_sets() > 0)
	  F |= it->cond();
      }

    return new slap_state(init_tgba,
			  model_.leastPreTestFixpoint (model_.getInitialState(), F));
  }


  class tgba_no_succ_iterator final: public twa_succ_iterator {

    state* dst() const override {
      return 0;
    }


    /// \brief Get the condition on the transition leading to this successor.
    ///
    /// This is a boolean function of atomic propositions.
    bdd cond() const override {
      return bddfalse;
    }

    /// \brief Get the acceptance conditions on the transition leading
    /// to this successor.
    acc_cond::mark_t acc() const override {
      return {};
    }
    bool first() override {
      return false;
    };
    bool next() override {
      return false;
    };
    bool done() const override {
      return true;
    }
  };

  spot::twa_succ_iterator*
  slap_tgba::succ_iter(const state* local_state) const
  {
    // test for div case
    const slap_div_state* d = dynamic_cast<const slap_div_state*>(local_state);
    if (d) {
      return new slap_div_succ_iterator(get_dict(), d, d->get_condition(), d->get_acceptance());
    }


    // else it should be a normal slap state
    const slap_state* s = dynamic_cast<const slap_state*>(local_state);
    assert(s);


    twa_succ_iterator* li = left_->succ_iter(s->left());
    // Test whether the tgba state is "terminal", i.e. we might end it right here.
    if ( isFullAcceptingState(li, s->left()) ) {
      trace << "could use FSLTL algo !!" << std::endl;
      its::fsltlModel::trans_t nextAccs;
      its::Transition all = its::State::null;

      typedef std::map<std::string,its::Transition> accToTrans_t;
      typedef accToTrans_t::iterator accToTrans_it;
      accToTrans_t accToTrans;

      for ( li->first(); !li->done() ; li->next() ) {

	if ( isSelfLoop(li,s->left())) {
	  // compute toadd : the transition relation corresponding to this arc
	  its::Transition apcond = model_.getSelector(li->cond());
	  its::Transition toadd = model_.getNextRel () & apcond;

	  all = all + toadd;

	  labels_t accs = its::TgbaType::getAcceptanceSet (li->acc());
	  for (labels_it acc = accs.begin() ; acc != accs.end() ; ++acc) {
	    accToTrans_it accit = accToTrans.find(*acc);
	    if (accit == accToTrans.end()) {
	      // first occurrence
	      accToTrans [*acc] = toadd;
	    } else {
	      accit->second = accit->second + toadd;
	    }
	  }
	}
      }

      for (accToTrans_it accit = accToTrans.begin() ; accit != accToTrans.end() ; ++accit ) {
	nextAccs.push_back(accit->second);
	trace << "For acceptance condition  :" <<  accit->first << std::endl ;
      }


      its::State scc = its::fsltlModel::findSCC_owcty (all, nextAccs, s->right());

      if (scc == its::State::null) {
	if (fsType_ == sogits::FST
	    || (fsType_ == sogits::FSA &&  isTerminalState(li, s->left()) )) {
	  return new tgba_no_succ_iterator();
	} else if ( fsType_ == sogits::FSA ) {
	  // default back to basic behavior
	  return new slap_succ_iterator(left_.get(), s->left(), li, model_, s->right(),fsType_);
	}
      } else {
	return new slap_div_succ_iterator(get_dict(), s, bddtrue, left_->acc().all_sets());
      }
    }

    // std::cerr << "Building succ_iter from state : " << left_->format_state (s->left()) << std::endl;
    return new slap_succ_iterator(left_.get(), s->left(), li, model_, s->right(),fsType_);
  }

  bool slap_tgba::isSelfLoop(const twa_succ_iterator * it,
			     const state * source) const {
    bool ret = false;
    const state * q2 = it->dst();
    if (q2->compare(source) == 0) {
      ret = true;
    }
    q2->destroy();
    return ret;
  }

  // only consider a state terminal when it has no successors except themself
  bool slap_tgba::isTerminalState (twa_succ_iterator * it, const state * source) const {
    for ( it->first(); !it->done() ; it->next() ) {
	if (! isSelfLoop(it,source)) {
	  trace << "State " << left_->format_state(source) << "is not terminal" << std::endl;
	  return false;
	}
      }
      trace << "State " << left_->format_state(source) << "IS terminal" << std::endl;
      return true;
  }

  bool slap_tgba::isFullAcceptingState (twa_succ_iterator * it, const state * source) const {
    // FST => only consider a state terminal when it has no successors except themself
    if ( fsType_==sogits::FST) {
      return isTerminalState(it,source);

      // FSA => a state which carries all acceptance conditions on self loops is considered terminal
    } else if ( fsType_==sogits::FSA ) {

      // Compute in slAcc the set of acceptance conditions labeling self-loops
      spot::acc_cond::mark_t slAcc = 0U;

      for ( it->first(); !it->done() ; it->next() ) {
	if ( isSelfLoop(it,source) ) {
	  // its a self loop,
	  // store acceptance conditions
	  slAcc |= it->acc();
	}
      }

      if ( left_->acc().accepting(slAcc)) {
	trace << "State " << left_->format_state(source) << "IS FSA-terminal" << std::endl;
	return true;
      } else {
	return false;
      }

      // Otherwise, use plain SLAP algo, no FS search enabled.
    } else {
      return false;
    }
  }

  std::string
  slap_tgba::format_state(const state* state) const
  {
    const slap_div_state* d = dynamic_cast<const slap_div_state*>(state);
    if (d) {
      std::ostringstream os;
      os // <<  left_->format_state(d->get_left_state())
	 << " * div_state(";
      spot::bdd_print_formula(os, dict_, d->get_condition()) << ")";

      return os.str();
    }


    const slap_state* s = dynamic_cast<const slap_state*>(state);
    assert(s);

    // debugging...
//     its::Transition nextRel = model_.getNextRel();
//     its::State div = fixpoint( nextRel * its::Transition::id ) (s->right());
//     bool isDiv = div != its::State::null;
    std::stringstream ss;
    model_.getType()->printState(s->right(), ss,15) ;

    return (left_->format_state(s->left())
	    + " * "
	    + " SDD size: " + to_string(s->right().nbStates())
//	    + (isDiv ? " div "  : " not div ")
	    + " hash:" + to_string(s->right().hash()) + ss.str() );
  }

  state*
  slap_tgba::project_state(const state* s, const const_twa_ptr& t) const
  {
    const slap_state* s2 = dynamic_cast<const slap_state*>(s);
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

  slap_div_state::slap_div_state(const bdd& c, acc_cond::mark_t a)
    : cond(c), acc(a) {
  }

  int slap_div_state::compare(const state* other) const {
    const slap_div_state* m = dynamic_cast<const slap_div_state*>(other);
    if (!m)
      return -1;
    return cond.id() - m->cond.id();
  }

  size_t slap_div_state::hash() const {
    return wang32_hash(cond.id());
  }

  spot::state* slap_div_state::clone() const {
    return new slap_div_state(*this);
  }

  const bdd& slap_div_state::get_condition() const {
    return cond;
  }

  spot::acc_cond::mark_t slap_div_state::get_acceptance() const {
    return acc;
  }

  std::ostream & slap_div_state::print (std::ostream & os) const {
    return (os << "SlapDivState " << std::endl);
  }


  slap_div_succ_iterator::slap_div_succ_iterator(const spot::bdd_dict_ptr& d,
						 const spot::state* s,
						 const bdd & cond,
						 spot::acc_cond::mark_t acc)
    : dict(d), state(s), done_(false), cond_(cond), acc_(acc)
  {
  }

  bool slap_div_succ_iterator::first() {
    done_ = false;
    return true;
  }

  bool slap_div_succ_iterator::next() {
    done_ = true;
    return false;
  }

  bool slap_div_succ_iterator::done() const {
    return  done_;
  }

  const spot::state* slap_div_succ_iterator::dst() const {
    assert(!done_);
    trace << "FIRING : " << format_transition() << std::endl;
    trace << "FROM a div state" << std::endl << std::endl;
    return new slap_div_state(cond_,acc_);
  }

  bdd slap_div_succ_iterator::cond() const {
    assert(!done());
    return cond_;
  }

  acc_cond::mark_t slap_div_succ_iterator::acc() const {
    assert(!done());
    return acc_;
  }

  std::string slap_div_succ_iterator::format_transition() const {
    assert(!done());
    std::ostringstream os;
    spot::bdd_print_formula(os, dict, cond_);
    return "div(" + os.str() + ")";
  }

  slap_div_succ_iterator::slap_div_succ_iterator(const slap_div_succ_iterator& s) {
    assert(false);
  }

  slap_div_succ_iterator& slap_div_succ_iterator::operator=(const slap_div_succ_iterator& s) {
    assert(false);
    return *this;
  }

}
