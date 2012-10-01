#ifndef __GAL__TYPE__
#define __GAL__TYPE__

#include "TypeBasics.hh"
#include "GAL.hh"
#include "TypeVisitor.hh"

// forward declaration
namespace dve2GAL {
  class dve2GAL;
}

namespace its {

  
class GALType : public TypeBasics {

  const GAL * gal_;
  /** if true, add a transition /\_{t \in trans} (! t.guard)
   ** this transition makes a self-loop on deadlocks, allowing to have the same LTL semantics as divine */
  bool stutterOnDeadlock;

  // support function to builda Hom from a GuardedAction (using current varOrder)
  GHom buildHom(const GuardedAction & it) const ;
public :
  GALType (const GAL * gal):gal_(gal){}

  void setStutterOnDeadlock (bool s) { stutterOnDeadlock = s; }
  
  Label getName() const { return gal_->getName(); }

  /** the set InitStates of designated initial states (a copy)*/
  labels_t getInitStates () const  { return labels_t(1,"init"); }
  
  /** the set T of public transition labels (a copy)*/
  labels_t getTransLabels () const ;

  /** state and transitions representation functions */
  /** Local transitions */
  Transition getLocals () const;

  
  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void getNamedLocals (namedTrs_t & ntrans) const ;

  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  Transition getSuccs (const labels_t & tau) const ;

  /** To obtain a representation of a labeled state */
  State getState(Label stateLabel) const ;

  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  virtual Transition getAPredicate (Label predicate) const ;

  /* delegated */
  std::ostream & print (std::ostream & os) const { os << *gal_ ; return os ; }

  virtual void printState (State s, std::ostream & os) const { 
    TypeBasics::printDDDState (s,os,*getVarOrder());
  }

  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State getPotentialStates(State reachable) const;


  /** Allow to visit the underlying type definition */
  void visit (class TypeVisitor * visitor) const {
    visitor->visitGAL(*gal_);
  }

  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition observe (labels_t obs, State potential) const; 

  labels_t getVarSet () const ;
  
};

class GALDVEType : public GALType {
  struct dve2GAL::dve2GAL * dve;
public:
  GALDVEType (const GAL *, dve2GAL::dve2GAL *);
  GALDVEType (const GAL *);
  
  void setDVE2GAL (dve2GAL::dve2GAL *);
  
  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  This class supports the original LTL syntax from DVE/BEEM.
   *  Especially, it interprets correctly "P0.CS" as "P0.state='index of CS'".
   */
  Transition getAPredicate (Label predicate) const ;
};

class GALTypeFactory {
public:
  static GALType * createGALType (const GAL *);
  static GALType * createGALDVEType (Label, bool stutterOnDeadlock);
};

}

#endif
