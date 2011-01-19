/*
 * EtfITS.hh
 *
 *  Created on: 7 déc. 2008
 *      Author: yann
 */

#ifndef ETFTYPE_HH_
#define ETFTYPE_HH_

#include "TypeBasics.hh"
#include "TypeVisitor.hh"

extern "C" {
#include "etf-util.h"
}


namespace its {

/** A etf adapter matching the ITS type contract.
 *  Uses (friendly) delegation on Etf class */
class EtfType : public  TypeBasics {
  // The concrete storage class
  etf_model_t etfmodel;
  lts_type_t ltstype;

  vLabel name;
 protected :
  // returns the set of component instance names 
  labels_t getVarSet () const;
public :
  EtfType (Label path) ;



	/** the set InitStates of designated initial states (a copy)*/
	labels_t getInitStates () const ;

	/** the set T of public transition labels (a copy)*/
	labels_t getTransLabels () const ;

	/** state and transitions representation functions */
	/** Local transitions */
	Transition getLocals () const ;

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
  Transition getPredicate (Label predicate) const ;


  /* delegated */
  std::ostream & print (std::ostream & os) const {  return os; /* return comp_.print(os); */  }  

  void printState (State s, std::ostream & os) const;
  
  
  Label getName() const { return name; }

  void visit (class TypeVisitor * visitor) const { /* visitor->visitEtf(comp_);*/ }
};

} // namespace

#endif /* ETFITS_HH_ */
