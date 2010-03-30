#ifndef __JSONPNET__SET__TYPE__
#define __JSONPNET__SET__TYPE__

#include "TypeBasics.hh"
#include "PNet.hh"
#include "TypeVisitor.hh"
#include "ITSModel.hh"
#include "parser_json/hierarchie.hh"

namespace its {


class JsonPNetType : public TypeBasics {
  // The concrete model declaration storage class
  PNet net_;
  // The sandbox model to build the final representation in
  mutable ITSModel model_;
  // The JSON hierarchy
  json::Hierarchie hier_;
  // real type for delegation
  mutable pType concrete_;
  // lazy build of concrete
  pType getConcrete () const ;
protected :
  labels_t getVarSet () const { return labels_t(); }
  const PNet & getNet() const { return net_; }
public :

  JsonPNetType (const PNet & net, const json::Hierarchie & hier) : net_(net), hier_(hier), concrete_(NULL) {}
  virtual ~JsonPNetType() {}

  /** the set InitStates of designated initial states (a copy)*/
  labels_t getInitStates () const { return getConcrete()->getInitStates(); }

  /** the set T of public transition labels (a copy)*/
  virtual labels_t getTransLabels () const { return getConcrete()->getTransLabels(); }

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
  std::ostream & print (std::ostream & os) const { return net_.print(os); }  
  Label getName() const { return net_.getName(); }

  virtual void printState (State s, std::ostream & os) const { os << "Please implement pretty state print for JsonPNet " << std::endl; }

    /** Allow to visit the underlying type definition */
  void visit (class TypeVisitor * visitor) const {
    visitor->visitPNet(net_);
  }
};



} // namespace

#endif

