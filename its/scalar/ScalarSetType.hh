#ifndef __SCALAR__SET__TYPE__
#define __SCALAR__SET__TYPE__

#include "its/TypeBasics.hh"
#include "its/scalar/ScalarSet.hh"
#include "its/TypeVisitor.hh"
#include "its/ITSModel.hh"

namespace its {


  // Strategies
  class RepresentationStrategy {
  public :
    virtual ~RepresentationStrategy () {};
    virtual vLabel buildRepresentation () = 0; 
    virtual int getTypeSize(Label name) const = 0;
  };


class ScalarSetType : public TypeBasics {
  // The sandbox model to build the final representation in
  mutable ITSModel model_;
  // strategy
  mutable RepresentationStrategy * strat_;
  // real type for delegation
  mutable pType concrete_;
  // lazy build of concrete
  pType getConcrete () const ;
protected :
  // The concrete model declaration storage class
  ScalarSet *comp_;

  labels_t getVarSet () const { return labels_t(); }
  const ScalarSet & getComp() const { return *comp_; }
  virtual void setComp(const ScalarSet & comp) { 
    comp_ = new ScalarSet(comp); 
  }
  // protected constructor for use in circular set : does NOT initialize comp_
  ScalarSetType():strat_(NULL) { setStrategy(DEPTH1); }

  // returns the variable name in the current representation.
  // var is supposed to start by an integer designating an instance in the set
  // this integer will be replaced by a qualified name half0.half1. etc... 
  // appropriate to the represetation strategy.
  // The end of "var" string is left unchanged
  vLabel resolveVariableName (Label var) const;
public :
  // factory behavior
  virtual void setStrategy (scalarStrategy strat, int parameter=1) ;



  ScalarSetType (const ScalarSet & c) : strat_(NULL) { setStrategy(DEPTH1); setComp(c);};
  virtual ~ScalarSetType() { delete strat_; delete comp_; }

  /** the set InitStates of designated initial states (a copy)*/
  labels_t getInitStates () const ;

  /** the set T of public transition labels (a copy)*/
  virtual labels_t getTransLabels () const ;

  /** state and transitions representation functions */
  /** Local transitions */
  Transition getLocals () const {  return getConcrete()->getLocals() ;}

  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void getNamedLocals (namedTrs_t & ntrans) const { getConcrete()->getNamedLocals(ntrans); }

  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  Transition getSuccs (const labels_t & tau) const  {  return getConcrete()->getSuccs(tau) ;};
  
  /** To obtain a representation of a labeled state */
  State getState(Label stateLabel) const  {  return getConcrete()->getState(stateLabel) ;};

    /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition getAPredicate (Label predicate) const ;
  
  /* delegated */
  std::ostream & print (std::ostream & os) const { 
    getComp().print(os); 
    // force representation construction
    getConcrete();
    os << "Encoded as a submodel :\n" << model_ << std::endl;
    return os;
  }  
  Label getName() const { return getComp().getName(); }

  virtual long printState (State s, std::ostream & os, long limit) const { return getConcrete()->printState(s,os,limit); }

  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State getPotentialStates(State reachable) const {
    return getConcrete()->getPotentialStates(reachable);
  }
    /** Allow to visit the underlying type definition */
  void visit (class TypeVisitor * visitor) const {
    visitor->visitScalar(getComp());
  }

  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition observe (labels_t obs, State potential) const ;


};



} // namespace

#endif

