#ifndef ___SOGITS__HH__
#define ___SOGITS__HH__

#include "Ordering.hh"

#include "ITSModel.hh"
// BuDDy package
#include "bdd.h"



class sogIts {

  const its::ITSModel & model;

  // Defines a cache for bdd representing a boolean formula to Transition
  typedef  hash_map<int,its::Transition>::type formCache_t;
  typedef formCache_t::accessor formCache_it;
  mutable formCache_t formulaCache;


   // Defines a mapping from atomic property name to bdd variable index
  its::VarOrder apOrder_;

public :
  sogIts (const its::ITSModel & m) : model(m) {};

  // Atomic properties handling primitives
  // return a selector corresponding to the boolean formula over AP encoded as a bdd.
  its::Transition getSelector(bdd aps) const;

  // Other version to act on any given type, instead of the main instance of the Model object
  // return a selector corresponding to the boolean formula over AP encoded as a bdd.
  its::Transition getSelector(bdd aps, its::pType type) const;


  // Saturate the provided states, while preserving the truth value of "cond".
  // The truth value of cond need not be homogeneous in the provided states.
  // fixpoint ( hcond & locals() + id ) ( hcond(s) )
  // Where hcond represents getSelector(cond)
  its::State leastPostTestFixpoint ( its::State init, bdd cond ) const ;

  // Saturate the provided states, while ensuring that only states with
  // "cond" or their successors are kept.
  // The truth value of cond need not be homogeneous in the provided states.
  // fixpoint ( locals() & hcond + id ) (s)
  its::State leastPreTestFixpoint ( its::State init, bdd cond ) const ;

  // Find successors of states satisfying "cond" in the provided aggregate "init"
  its::State succSatisfying ( its::State init, bdd cond) const;

  // Return the set of divergent states in a set, using  (hcond & next) as transition relation
  its::State getDivergent (its::State init, bdd cond) const;

  its::State getInitialState () const { return model.getInitialState() ; }

  its::Transition getNextRel () const {
    return model.getNextRel();
  }

  // Set an observed atomic proposition : the string corresponding to the AP identifier is then related to the bdd var of index
  bool setObservedAP (Label ap, int bddvar) {
    return apOrder_.addVariable (ap,bddvar);
  }


};

#endif
