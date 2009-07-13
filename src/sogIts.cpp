#include "sogIts.hh"

using namespace its;


//#define trace std::cerr
#define trace if (0) std::cerr

// Atomic properties handling primitives
// return a selector corresponding to the boolean formula over AP encoded as a bdd.
Transition sogIts::getSelector(bdd aps) const {
  formCache_it it ;
  if ( formulaCache.find(it,aps.id()) ) 
    // cache hit
    return it->second;
  
  if ( aps == bddtrue ) {
    return Transition::id ;
  } else if (aps == bddfalse ) {
    return State::null;
  } else {
    // a "real" node
    int bvar = bdd_var(aps);
    Label prop = apOrder_.getLabel(bvar);
    
    labels_t tau;
    tau.push_back(prop);
    Transition hcond = model.getInstance()->getType()->getSuccs(tau);
    trace << "aps = " << aps  << std::endl;
    //      std::cerr << "Type = " << * (getInstance()->getType())  << std::endl;
    trace << "prop = " << prop << std::endl ;
    trace << "hcond = "  << hcond << std::endl ;
    
    Transition ret ;
    if ( bdd_high(aps) == bdd_low(aps) ) {
      // This AP is a don't care in this part of the boolean formula
      ret = getSelector(bdd_high(aps));
    } else {
      ret = ITE(hcond, getSelector(bdd_high(aps)),  getSelector(bdd_low(aps)) ); 
    }
    formulaCache.insert(it, aps.id());
    it->second = ret;
    
    trace << "Produced transition :" << ret << std::endl;
    return ret;
    
  }
}


// Saturate the provided states, while preserving the truth value of "cond"
// The truth value of cond need not be homogeneous in the provided states.
// fixpoint ( hcond & locals() + id ) ( hcond(s) )
// Where hcond represents getSelector(cond)
its::State sogIts::leastPostTestFixpoint ( State init, bdd cond ) const {
  Transition hcond = getSelector(cond);
  Transition hnext = getNextRel();
  
  Transition sat = fixpoint ( (hcond &  hnext) + Transition::id ) & hcond;
  trace << "Saturate (post) least fixpoint under conditions : " << sat << std::endl;
  return sat(init);
}

// Saturate the provided states, while ensuring that only states with
// "cond" or their successors are kept
// fixpoint ( locals() & hcond + id ) (s)
its::State sogIts::leastPreTestFixpoint ( its::State init, bdd cond ) const {
  Transition hcond = getSelector(cond);
  Transition hnext = getNextRel();
  
  Transition sat = fixpoint ( ( hnext & hcond ) + Transition::id );
  
  trace << "Saturate (pre) least fixpoint under conditions : " << sat << std::endl;
  return sat(init);

}


// Find successors of states satisfying "cond" in the provided aggregate "init"
State  sogIts::succSatisfying ( its::State init, bdd cond) const {
  return (getNextRel()  & getSelector(cond)) (init);
}

// Return the set of divergent states in a set, using  (hcond & next) as transition relation
State sogIts::getDivergent (State init, bdd cond) const {
  return fixpoint ( (getSelector(cond) & model.getNextRel()) * Transition::id ) (init);
}

