#include "operators.hh"

namespace ctl {
  // compute states matching : TRUE
  its::State computeTrue (const CTLChecker & checker) { return its::State::null; }

  // compute states matching : FALSE
  its::State computeFalse (const CTLChecker & checker) { return its::State::null; } ;

  // compute initial states 
  its::State computeInitial (const CTLChecker & checker) { return its::State::null; };
  
  // compute states matching a predicate p
  its::State computeAtomicPredicate (Label pred, const CTLChecker & checker) { return its::State::null; }


  //////////// Boolean cases

  // p AND q
  its::State computeAnd (Ctlp_Formula_t * p, Ctlp_Formula_t * q, const CTLChecker & checker) { return its::State::null; } 

  // p OR q
  its::State computeOr (Ctlp_Formula_t * p, Ctlp_Formula_t * q, const CTLChecker & checker) { return its::State::null; }

  // NOT(p)
  its::State computeNot (Ctlp_Formula_t * p, const CTLChecker & checker) { return its::State::null; }

  // p => q  (p implies q)
  its::State computeImplies (Ctlp_Formula_t * p, Ctlp_Formula_t * q, const CTLChecker & checker) { return its::State::null; }

  ////////////// Temporal operators
  
  // compute states matching : EX p
  its::State computeEX (Ctlp_Formula_t * p,  const CTLChecker & checker) { return its::State::null; }

  // compute states matching : E ( p U q )
  its::State computeEU (Ctlp_Formula_t * p,  Ctlp_Formula_t * q, const CTLChecker & checker) { return its::State::null; }

  // compute states matching : EG p
  its::State computeEG (Ctlp_Formula_t * p, const CTLChecker & checker) { return its::State::null; }


}
