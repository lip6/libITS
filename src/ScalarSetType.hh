#ifndef __SCALAR__SET__TYPE__
#define __SCALAR__SET__TYPE__

#include "TypeBasics.hh"
#include "ScalarSet.hh"
#include "TypeVisitor.hh"
#include "ITSModel.hh"

namespace its {


  // Strategies
  class RepresentationStrategy {
  public :
    virtual vLabel buildRepresentation () = 0; 
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

  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  Transition getSuccs (const labels_t & tau) const  {  return getConcrete()->getSuccs(tau) ;};
  
  /** To obtain a representation of a labeled state */
  State getState(Label stateLabel) const  {  return getConcrete()->getState(stateLabel) ;};
  
  /* delegated */
  std::ostream & print (std::ostream & os) const { return getComp().print(os); }  
  Label getName() const { return getComp().getName(); }

  virtual void printState (State s, std::ostream & os) const { os << "Please implement pretty state print for Scalar Set" << std::endl; }

    /** Allow to visit the underlying type definition */
  void visit (class TypeVisitor * visitor) const {
    visitor->visitScalar(getComp());
  }
};



} // namespace

#endif

