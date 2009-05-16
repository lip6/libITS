#include "sogIts.hh"

using namespace its;


#define trace std::cerr
// #define trace if (0) std::cerr

// Atomic properties handling primitives
// return a selector corresponding to the boolean formula over AP encoded as a bdd.
Transition bddITSModelAdapter::getSelector(bdd aps) const {
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
    Transition ret = ITE(hcond, getSelector(bdd_high(aps)),  getSelector(bdd_low(aps)) ); 
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
State bddITSModelAdapter::leastFixpoint ( State init, bdd cond ) const {
  Transition hcond = getSelector(cond);
  Transition hnext = model.getNextRel();
  
  Transition sat = fixpoint ( (hcond &  hnext) + Transition::id ) & hcond;
  trace << "Saturate least fixpoint under conditions : " << sat << std::endl;
  return sat(init);
}

// Return the set of divergent states in a set, using  (hcond & next) as transition relation
State bddITSModelAdapter::getDivergent (State init, bdd cond) const {
  return fixpoint ( (getSelector(cond) & model.getNextRel()) * Transition::id ) (init);
}

