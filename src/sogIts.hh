#ifndef ___SOGITS__HH__
#define ___SOGITS__HH__

#include "Ordering.hh"

#include "ITSModel.hh"
// BuDDy package
#include <bddx.h>


namespace sogits {
  enum sog_product_type {
    FS_OWCTY, // < The fully symbolic approach: One-Way catch them Young. Encode tgba as an ITS.
    FS_EL, // < The fully symbolic approach: Emerson-Lei variant. Encode tgba as an ITS.
    BCZ99, // < The Biere,Clarke,Zhu'99 algorithm (single step SOG)
    PLAIN_SOG, // < The basic algorithm with static parameters.
    SLAP_NOFS, // < The (Symbolic) Local Obs Graph algorithm
    SLAP_FST, // < The (Symbolic) Local Obs Graph algorithm with Terminal states FSLTL test.
    SLAP_FSA, // < The (Symbolic) Local Obs Graph algorithm with Accepting states FSLTL test.
    SOP, // < The Symbolic Observation Product algorithm
    FS_OWCTY_TGTA, // < The TGTA fully symbolic approach: One-Way catch them Young. Transform a TGBA into TGTA and then encode the TGTA as an ITS.
    SOG_TGTA, // < The basic algorithm with static parameters and using TGTA instead of TGBA.
    SLAP_TGTA, // < The (Symbolic) Local Obs Graph algorithm using TGTA instead of TGBA.
    SOP_TGTA, // < The Symbolic Observation Product algorithm using TGTA instead of TGBA.
    SLAP_DTGTA, // < The (Symbolic) Local Obs Graph algorithm using DTGTA (Don't care TGTA) instead of TGBA.
    SOP_DTGTA  // < The Symbolic Observation Product algorithm using DTGTA (Don't care TGTA) instead of TGBA.
  };
}



class sogIts {

  const its::ITSModel & model;

  // Defines a cache for bdd representing a boolean formula to Transition
  typedef  hash_map<int,its::Transition>::type formCache_t;
  typedef formCache_t::accessor formCache_it;
  mutable formCache_t formulaCache;


   // Defines a mapping from atomic property name to bdd variable index
  its::VarOrder apOrder_;

  // True if for ascending compatibility issues, atomic properties should be reinterpreted
  // as if they were just the names of places, instead of comparisons.
  // e.g. reinterpret AP  "Idle0" as "Idle0 = 1"
  bool isPlaceSyntax;
public :
  sogIts (const its::ITSModel & m) : model(m),isPlaceSyntax(false) {};

  // True if for ascending compatibility issues, atomic properties should be reinterpreted
  // as if they were just the names of places, instead of comparisons.
  // e.g. reinterpret AP  "Idle0" as "Idle0 = 1"
  void setPlaceSyntax (bool val) { isPlaceSyntax = val; }
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

  its::pType getType() const { return model.getInstance()->getType() ; }

  its::Transition getNextRel () const {
    return model.getNextRel();
  }

  // Set an observed atomic proposition : the string corresponding to the AP identifier is then related to the bdd var of index
  bool setObservedAP (Label ap, int bddvar) {
    return apOrder_.addVariable (ap,bddvar);
  }


};

#endif
