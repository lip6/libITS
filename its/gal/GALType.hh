#ifndef __GAL__TYPE__
#define __GAL__TYPE__

#include "its/TypeBasics.hh"
#include "its/gal/GAL.hh"
#include "its/gal/GALOrder.hh"
#include "its/TypeVisitor.hh"
#include "its/gal/GALVarOrderHeuristic.hh"

// forward declaration
namespace dve2GAL {
  struct dve2GAL;
}

namespace its {

class GALType : public TypeBasics {

  const GAL * gal_;
  /** if true, add a transition /\_{t \in trans} (! t.guard)
   ** this transition makes a self-loop on deadlocks, allowing to have the same LTL semantics as divine */
  bool stutterOnDeadlock;
  /** the strategy to be used in the determination of the var order
   ** cf. GALVarOrderHeuristic.hh
   **/
  orderHeuristicType strat_;

  mutable GalOrder * go_;
  // support function to builda Hom from a GuardedAction (using current varOrder)
public: // \todo public for access from libltl, is it a good idea?
  GHom buildHom(const GuardedAction & it) const ;
  GHom buildHom(const Statement & it) const ;
  GHom getSuccsHom (const labels_t & tau) const ;
  friend class HomBuilder;
public :
  GALType (const GAL * gal):gal_(gal), strat_(DEFAULT), go_(NULL) {}

  void setStutterOnDeadlock (bool s) { stutterOnDeadlock = s; }
  void setOrderStrategy (orderHeuristicType s) { strat_ = s; }

  // For Gal order
  const GalOrder * getGalOrder() const {
    if (go_ == NULL) {
      go_ = new GalOrder (getVarOrder());
    }
    return go_;
  }

  // simple getter of the pointer to the GAL system
  const GAL * get_gal () const { return gal_; }
  
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
  Transition getPredicate (char * predicate) const;
  /// Atomic predicates for GAL use normal parser
  Transition getAPredicate (Label predicate) const { 
    char buff [predicate.size()+1];
    strcpy(buff,predicate.c_str());
    return getPredicate (buff); 
  }
  /// the real state predicate function parsing function
  virtual BoolExpression getBPredicate (Label pred) const;

  /* delegated */
  std::ostream & print (std::ostream & os) const { os << *gal_ ; return os ; }

  virtual long printState (State s, std::ostream & os,long limit) const { 
    return TypeBasics::printDDDState (s,os,limit,*getVarOrder());
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

  void getVarIndex(varindex_t & index, Label vname) const {
    // to access inside the SDD
    index.push_back(0);
    // actual var index
    TypeBasics::getVarIndex(index,vname);
  }

  virtual bool setVarOrder (labels_t ord) const {
    go_ = NULL;
    return TypeBasics::setVarOrder(ord);
  }
  
};

class GALDVEType : public GALType {
  struct dve2GAL::dve2GAL * dve;
  
  /// a helper function that turns the atomic propositions in dve syntax to gal syntax
  /// Especially, it turns "P0.CS" into "P0.state='index of CS'".
  vLabel predicate_dve2gal (Label) const;
public:
  GALDVEType (const GAL *, dve2GAL::dve2GAL *);
  GALDVEType (const GAL *);
  
  void setDVE2GAL (dve2GAL::dve2GAL *);
  
  bool setVarOrder (labels_t) const;
  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  This class supports the original LTL syntax from DVE/BEEM.
   *  Especially, it interprets correctly "P0.CS" as "P0.state='index of CS'".
   */
  BoolExpression getBPredicate (Label pred) const ;
};


}

#endif
