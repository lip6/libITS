#include <cassert>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "tgba/bddprint.hh"


#include "apiterator.hh"

#include "bczCSD99.hh"

#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


namespace sogits {

  // build the agregate inductively defined by
  //  (ap&Trans + id)^*  & ap (m)
  bcz_state::bcz_state(const sogIts & model, const GSDD& entryStates, bdd bddAP) 
    : spot::state(),condition(bddAP) {
    states = model.getSelector(bddAP) (entryStates);
    succ = model.getNextRel() (states);
    
    trace << "------\nBuilt bczstate from " << entryStates.nbStates() << " initial states ;" << std::endl ;
    trace << "With AP =" << bddAP << std::endl;
    trace << "Obtained :" << *this ;
    trace << "(succ == states) =" << (succ == states) << std::endl;
    trace << "------\n";
  } 
  
  GSDD bcz_state::get_succ() const {
    return  succ;
  }
  

  int bcz_state::compare(const state* other) const {
    const bcz_state* m = dynamic_cast<const bcz_state*>(other);
    if (!m)
      return 1;
    return (states < m->states) ? -1 : 
      (states == m->states) ?  0 : 1 ;
  } //

  size_t bcz_state::hash() const {
    return states.hash();
  } //

  spot::state* bcz_state::clone() const {
    return new bcz_state(*this);
  } //

  const GSDD & bcz_state::get_states() const {
    return states;
  } //

  std::ostream & bcz_state::print (std::ostream & os) const {
    return (os << "(BczState : size =" << get_states().nbStates() << " succ.size = " << get_succ().nbStates() << ")" << std::endl);
  }


bcz_succ_iterator::bcz_succ_iterator(const sogIts& m, const bcz_state& s)
  : model(m), 
    from(s), 
    it(APIteratorFactory::create()), 
    succstates(from.get_succ()), 
    current_succ(NULL) {
  // set status of iterator to done() initially

  // succstates : initialize Ext = states that serve as seed for successors
  trace << "Built bcz_succ_iterator : " << *this << std::endl;
}

  bcz_succ_iterator::~bcz_succ_iterator () {
    delete current_succ;
    delete it;
  }


  void bcz_succ_iterator::step() {
    // iterate until a non empty succ is found (or end reached)
    for (  ; ! it->done() ; it->next() ) {
      bcz_state s (model, succstates, it->current() );
      if ( s.get_states() != SDD::null ) {
	current_succ = new bcz_state(s);
	break;
      }
    }
  }

void bcz_succ_iterator::first() {

  /// position "it" at first of ap bdd set
  it->first();
  step();
}


void bcz_succ_iterator::next() {
  assert(!done());
  it->next();
  step();
} //

bool bcz_succ_iterator::done() const {
  return  it->done() ;
} //

spot::state* bcz_succ_iterator::current_state() const {
  assert(!done());
  trace << "FIRING : " << it->current() << std::endl;
  trace << "FROM " << from << std::endl;
  return new bcz_state(*current_succ);
} //

bdd bcz_succ_iterator::current_condition() const {
  assert(!done());
  trace << "Succ iter" << *this << " asked for current cond= " << from.get_condition() << std::endl;
  return from.get_condition();
} // 


bdd bcz_succ_iterator::current_acceptance_conditions() const {
  assert(!done());
  return bddfalse;
} //

  std::ostream & bcz_succ_iterator::print (std::ostream & os) const {
    return (os << "BczSuccIter : from =" << from << std::endl);
  }
  


} // namespace


std::ostream & operator << (std::ostream & os, const sogits::bcz_state &s) {
  return s.print(os);
}


std::ostream & operator << (std::ostream & os, const sogits::bcz_succ_iterator &s) {
  return s.print(os);
}




