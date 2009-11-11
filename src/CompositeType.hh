/*
 * CompositeITS.hh
 *
 *  Created on: 7 déc. 2008
 *      Author: yann
 */

#ifndef COMPOSITETYPE_HH_
#define COMPOSITETYPE_HH_

#include "TypeBasics.hh"
#include "Composite.hh"

namespace its {

/** A composite adapter matching the ITS type contract.
 *  Uses (friendly) delegation on Composite class */
class CompositeType : public  TypeBasics {
  // The concrete storage class
  Composite comp_;

  Transition getFullShom (const Synchronization & sync) const ;
 protected :
  // returns the set of component instance names 
  labels_t getVarSet () const;
public :
  CompositeType (const Composite & c) : comp_(c){};


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

  /* delegated */
  std::ostream & print (std::ostream & os) const { return comp_.print(os); }  
  Label getName() const { return comp_.getName(); }
};

} // namespace

#endif /* COMPOSITEITS_HH_ */
