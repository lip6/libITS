// Copyright (C) 2003, 2004, 2006, 2009 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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

#include "slap.hh"
#include <string>
#include <cassert>
#include "misc/hashfunc.hh"
#include "tgba/bddprint.hh"
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
  slap_succ_iterator::slap_succ_iterator(const tgba * aut, const spot::state * aut_state, tgba_succ_iterator* left, const sogIts & model, const its::State& right)
    : aut_(aut),
      aut_state_(aut_state),
      left_(left),
      model_(model),
      right_(right)
  {

    /** Test whether there is a self loop labeled with ALL acceptance conditions */
//     for (left_->first() ; ! left_->done() ; left_->next() ) {
//       if ( left_->current_state()->compare(aut_state) == 0 ) {
// //	std::cerr << "on arc :" << aut_->transition_annotation(left_) << std::endl;
// 	if ( left_->current_acceptance_conditions() ==  aut_->all_acceptance_conditions()  ) {
// 	  trace << "\ndetected condition !" << std::endl;
// 	}
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
    bdd ac = left_->current_acceptance_conditions();
    // The state reached in the tgba through this arc
    const state * q2 = left_->current_state();

    // Iterate over q2's successors, and add to F the atomic props which are on arcs "weaker" (wrt ac)
    bdd F = bddfalse;

    tgba_succ_iterator * it = aut_->succ_iter(q2);
    for ( it->first() ; ! it->done() ; it->next() ) {
      const state * dest = it->current_state();

      // Test self loop
      if ( dest->compare(q2) == 0 ) {
	// Test ac=>ac' (subsume the arc)
	bdd acprime = it->current_acceptance_conditions();
	if ( (ac & acprime) == acprime)
	  F |= it->current_condition();
      }
      dest->destroy();
    }
    delete it;

    return F;
  }

  void
  slap_succ_iterator::step_()
  {
    // Test if this TGBA arc is a self-loop without acceptance conditions, if the TGBA has acceptance conds
    if (aut_->all_acceptance_conditions() != bddfalse) {
      bdd curacc = left_->current_acceptance_conditions();
      if (curacc == bddfalse && left_->current_state()->compare(aut_state_) == 0) {
	dest_ = its::State::null;
	return;
      }
    }


    // progress to "entry" states of succ in ITS model
    bdd curcond = left_->current_condition();
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
	  // reached an appropriate non empty successor
	  return ;
	}
	// (else)
	// progress in tgba
	left_->next();
      }
  }

  void
  slap_succ_iterator::first()
  {
    left_->first();
    next_non_false_();
  }

  void
  slap_succ_iterator::next()
  {
    // Progress in the tgba succs
    left_->next();
    // search for a non empty succ aggregate
    next_non_false_();
  }

  bool
  slap_succ_iterator::done() const
  {
    return left_->done();
  }


  slap_state*
  slap_succ_iterator::current_state() const
  {
    return new slap_state(left_->current_state(),
			  dest_);
  }

  bdd
  slap_succ_iterator::current_condition() const
  {
    return left_->current_condition();
  }

  bdd slap_succ_iterator::current_acceptance_conditions() const
  {
    return left_->current_acceptance_conditions();
  }

  ////////////////////////////////////////////////////////////
  // slap_tgba

  /// \brief Constructor.
  /// \param left The left automata in the product.
  /// \param right The ITS model.
  slap_tgba::slap_tgba(const spot::tgba* left, const sogIts & right,sogits::FSTYPE fsType)
    : dict_(left->get_dict()), left_(left), model_(right), fsType_(fsType)
  {
    // register that we use the same bdd variables (dict) as the automaton.
    // these vars are unregistered in dtor
    dict_->register_all_variables_of(&left_, this);
  }

  slap_tgba::~slap_tgba()
  {
    dict_->unregister_all_my_variables(this);
  }

  state*
  slap_tgba::get_init_state() const
  {
    
    // The initial state of the tgba
    state * init_tgba = left_->get_init_state();
    // Iterate over init states successors, and add to F the atomic props which are on arcs without acceptance conds
    bdd F = bddfalse;

    tgba_succ_iterator * it = left_->succ_iter(init_tgba);
    for ( it->first() ; ! it->done() ; it->next() ) {
      const state * dest = it->current_state();
      // Test self loop
      if ( dest->compare(init_tgba) == 0 ) {
	// Test ac=0 (empty acceptance cond arcs)
	if (it->current_acceptance_conditions() == bddfalse && left_->all_acceptance_conditions() != it->current_acceptance_conditions()) {
	  F |= it->current_condition();
	}
      }
      dest->destroy();
    }
    delete it;


    return new slap_state(init_tgba,
			  model_.leastPreTestFixpoint (model_.getInitialState(), F));
  }


  class tgba_no_succ_iterator : public tgba_succ_iterator {

    state* current_state() const {
      return 0;
    }
    

    /// \brief Get the condition on the transition leading to this successor.
    ///
    /// This is a boolean function of atomic propositions.
    bdd current_condition() const {
      return bddfalse;
    }

    /// \brief Get the acceptance conditions on the transition leading
    /// to this successor.
    bdd current_acceptance_conditions() const {
      return bddfalse;
    }
    void first() {};
    void next() {};
    bool done() const {
      return true;
    }
  };

  spot::tgba_succ_iterator*
  slap_tgba::succ_iter(const state* local_state,
		       const state* global_state,
		       const tgba* global_automaton) const
  {
    // test for div case
    const slap_div_state* d = dynamic_cast<const slap_div_state*>(local_state);
    if (d) {
      return new slap_div_succ_iterator(get_dict(), d, d->get_condition(), d->get_acceptance());
    }
    

    // else it should be a normal slap state
    const slap_state* s = dynamic_cast<const slap_state*>(local_state);
    assert(s);


    tgba_succ_iterator* li = left_->succ_iter(s->left(),
					      global_state,
					      global_automaton);

    // Test whether the tgba state is terminal
    if ( (fsType_==sogits::FSA || fsType_==sogits::FST) && isTerminalState(li, s->left()) ) {
      trace << "could use FSLTL algo !!" << std::endl;
      its::fsltlModel::trans_t nextAccs;
      its::Transition all = its::State::null;

      typedef std::map<std::string,its::Transition> accToTrans_t;
      typedef accToTrans_t::iterator accToTrans_it;
      accToTrans_t accToTrans;

      for ( li->first(); !li->done() ; li->next() ) {

	// compute toadd : the transition relation corresponding to this arc
	its::Transition apcond = model_.getSelector(li->current_condition());
	its::Transition toadd = model_.getNextRel () & apcond;

	all = all + toadd;

	labels_t accs = its::TgbaType::getAcceptanceSet (li->current_acceptance_conditions(), left_);
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
      
      for (accToTrans_it accit = accToTrans.begin() ; accit != accToTrans.end() ; ++accit ) {
	nextAccs.push_back(accit->second);
	trace << "For acceptance condition  :" <<  accit->first << std::endl ;
      }
      
      
      its::State scc = its::fsltlModel::findSCC_owcty (all, nextAccs, s->right());

      if (scc == its::State::null) {
	return new tgba_no_succ_iterator();
      } else {
	return new slap_div_succ_iterator(get_dict(), s, bddtrue, left_->all_acceptance_conditions());
      }
    }
    
    // std::cerr << "Building succ_iter from state : " << left_->format_state (s->left()) << std::endl;
    return new slap_succ_iterator(left_, s->left(), li, model_, s->right());
  }

  bool slap_tgba::isTerminalState (tgba_succ_iterator * it, state * source) const {
    for ( it->first(); !it->done() ; it->next() ) {
      if (it->current_state()->compare(source) != 0) {
	trace << "State " << left_->format_state(source) << "is not terminal" << std::endl;
	return false;
      }
    }
    trace << "State " << left_->format_state(source) << "IS terminal" << std::endl;
    return true;
  }

  bdd
  slap_tgba::compute_support_conditions(const state* in) const
  {
    const slap_state* s = dynamic_cast<const slap_state*>(in);
    assert(s);
    return left_->support_conditions(s->left());
  }

  bdd
  slap_tgba::compute_support_variables(const state* in) const
  {
    const slap_state* s = dynamic_cast<const slap_state*>(in);
    assert(s);
    return left_->support_variables(s->left());
  }

  bdd_dict*
  slap_tgba::get_dict() const
  {
    return dict_;
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
    if ( s->right().nbStates() < 15 )
      model_.getType()->printState(s->right(), ss) ;

    return (left_->format_state(s->left())
	    + " * "
	    + " SDD size: " + to_string(s->right().nbStates()) 
//	    + (isDiv ? " div "  : " not div ")
	    + " hash:" + to_string(s->right().hash()) + ss.str() );
  }

  state*
  slap_tgba::project_state(const state* s, const tgba* t) const
  {
    const slap_state* s2 = dynamic_cast<const slap_state*>(s);
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
  slap_tgba::all_acceptance_conditions() const
  {
    return left_->all_acceptance_conditions();
  }

  bdd
  slap_tgba::neg_acceptance_conditions() const
  {
    return left_->neg_acceptance_conditions();
  }

  std::string
  slap_tgba::transition_annotation(const tgba_succ_iterator* t) const
  {
    const slap_succ_iterator* i =
      dynamic_cast<const slap_succ_iterator*>(t);
    assert(i);
    return left_->transition_annotation(i->left_);
  }


  slap_div_state::slap_div_state(const bdd& c, const bdd& a) : cond(c),acc(a) {
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

  const bdd& slap_div_state::get_acceptance() const {
    return acc;
  }

  std::ostream & slap_div_state::print (std::ostream & os) const {
    return (os << "SlapDivState " << std::endl);
  }

  
  slap_div_succ_iterator::slap_div_succ_iterator(const spot::bdd_dict* d,
						 const spot::state* s,
						 const bdd & cond,
						 const bdd & acc)
    : dict(d), state(s), done_(false), cond_(cond), acc_(acc)
  {
  }

  void slap_div_succ_iterator::first() {
    done_ = false;
  }

  void slap_div_succ_iterator::next() {
    if (!done_)
      done_ = true;
  }

  bool slap_div_succ_iterator::done() const {
    return  done_;
  }

  spot::state* slap_div_succ_iterator::current_state() const {
    assert(!done_);
    trace << "FIRING : " << format_transition() << std::endl;
    trace << "FROM a div state" << std::endl << std::endl;
    return new slap_div_state(cond_,acc_);
  }

  bdd slap_div_succ_iterator::current_condition() const {
    assert(!done());
    return cond_;
  }

  bdd slap_div_succ_iterator::current_acceptance_conditions() const {
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
