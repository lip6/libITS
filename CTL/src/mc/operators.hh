#ifndef __OPERATORS__HH__
#define __OPERATORS__HH__

#include "ctlCheck.hh"


// its::State = a SDD representing a *SET* of states
// Binary operators are overloaded: + is union, * is intersection, - is set difference
// State::null = empty set

// its::Transition = a transition relation, mapping an its::State to another its::State
// Paren operator is overloaded; to apply use :
// its::Transition t ; its::State s ; its::State s2 = t(s) ;


/*
// all these functions use CTLChecker that provides :

// initial set of states
its::State getInitialState () const;
// all reachable states
its::State getReachable () const;

// a selector, that retains states satisfying "ap"
its::Transition getAtomicPredicate (Label ap) const;
// the "Next" transition relation, forward one step.
its::Transition getNextRel () const ;
// the "Pred" transition relation, backward one step
its::Transition getPredRel () const;

// Compute the set of states satisfying a formula, and cache the result.
// Recursively relies on the functions in *this* file.
its::State getStateVerifying (Ctlp_Formula_t *formula) const;
*/

// This file separates the basic existential cases of CTL, to isolate them from the more complex and general treatment within ITS-CTL
// This helps to use the code base for exercises, i.e. ask to complete the implementation file.
namespace ctl {


  ////////////// Terminal cases
  
  // compute states matching : TRUE
  its::State computeTrue (const CTLChecker & checker) ;

  // compute states matching : FALSE
  its::State computeFalse (const CTLChecker & checker) ;

  // compute initial states 
  its::State computeInitial (const CTLChecker & checker) ;
  
  // compute states matching a predicate p
  its::State computeAtomicPredicate (Label pred, const CTLChecker & checker);


  //////////// Boolean cases

  // p AND q
  its::State computeAnd (Ctlp_Formula_t * p, Ctlp_Formula_t * q, const CTLChecker & checker) ;

  // p OR q
  its::State computeOr (Ctlp_Formula_t * p, Ctlp_Formula_t * q, const CTLChecker & checker) ;

  // NOT(p)
  its::State computeNot (Ctlp_Formula_t * p, const CTLChecker & checker) ;

  // p => q  (p implies q)
  its::State computeImplies (Ctlp_Formula_t * p, Ctlp_Formula_t * q, const CTLChecker & checker) ;

  ////////////// Temporal operators
  
  // compute states matching : EX p
  its::State computeEX (Ctlp_Formula_t * p,  const CTLChecker & checker) ;

  // compute states matching : E ( p U q )
  its::State computeEU (Ctlp_Formula_t * p,  Ctlp_Formula_t * q, const CTLChecker & checker) ;

  // compute states matching : EG p
  its::State computeEG (Ctlp_Formula_t * p, const CTLChecker & checker) ;

}

#endif
