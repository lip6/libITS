/*
 * CompositeITS.hh
 *
 *  Created on: 7 d�c. 2008
 *      Author: yann
 */

#ifndef COMPOSITETYPE_HH_
#define COMPOSITETYPE_HH_

#include "its/TypeBasics.hh"
#include "its/composite/Composite.hh"
#include "its/TypeVisitor.hh"

namespace its {

/** A composite adapter matching the ITS type contract.
 *  Uses (friendly) delegation on Composite class */
class CompositeType : public  TypeBasics {
  // The concrete storage class
  Composite comp_;

  Transition getFullShom (const Synchronization & sync) const ;


  // to avoid nested state if only one instance
  Transition skipLocalApply(Transition h, int index) const ;


  long recPrintState (State s, std::ostream & os, const VarOrder & vo, vLabel str,long limit, long pathSize) const;

  typedef std::pair<vLabel, vLabel> splitvar_t;
  // Returns a pair "instance name" "remains of string" by breaking on "." or resolving as nested variables.
  splitvar_t splitVar (vLabel predicate) const ;

 protected :
  // returns the set of component instance names 
  labels_t getVarSet () const;
public :
  CompositeType (const Composite & c) : comp_(c){};


  void addFlatVarSet (labels_t & vars, Label prefix) const ;

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
  Transition getAPredicate (Label predicate) const ;

  /** Return the index of a given variable in the representation, actually a vector of indices in SDD case.
   */
  void getVarIndex(varindex_t & index, Label vname) const;

  /* delegated */
  std::ostream & print (std::ostream & os) const { return comp_.print(os); }  

  long printState (State s, std::ostream & os, long limit) const;

  Label getName() const { return comp_.getName(); }

  void visit (class TypeVisitor * visitor) const { visitor->visitComposite(comp_); }

  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void getNamedLocals (namedTrs_t & ntrans) const ;
  
  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State getPotentialStates(State reachable) const ;

  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition observe (labels_t obs, State potential) const ;
  
};

} // namespace

#endif /* COMPOSITEITS_HH_ */
