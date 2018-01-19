/*
 * EtfITS.hh
 *
 *  Created on: 7 déc. 2008
 *      Author: yann
 */

#ifndef ETFTYPE_HH_
#define ETFTYPE_HH_

#include "its/TypeBasics.hh"
#include "its/TypeVisitor.hh"
#include <vector>

extern "C" {
#include "etf-util.h"
}

using std::vector;

namespace its {
/** A etf adapter matching the ITS type contract.
 *  Uses (friendly) delegation on Etf class */
class EtfType : public  TypeBasics {
  // The concrete storage class
protected :

  etf_model_t etfmodel;
  lts_type_t ltstype;

  vLabel name;
  
  // a helper function that parses the input ETF file using the primitives of LTSmin and produces the corresponding data structures.
  void compute_transitions (std::vector<class ETFTransition> & transitions) const ;

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

  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State getPotentialStates(State reachable) const;


	/** To obtain a representation of a labeled state */
	State getState(Label stateLabel) const ;

    /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition getAPredicate (Label predicate) const ;


  /* delegated */
  std::ostream & print (std::ostream & os) const {  return os; /* return comp_.print(os); */  }  

  long printState (State s, std::ostream & os,long limit) const;
  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void getNamedLocals (namedTrs_t & ntrans) const ;


  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition observe (labels_t obs, State potential) const ;
  
  Label getName() const { return name; }

  void visit (class TypeVisitor * visitor) const { /* visitor->visitEtf(comp_);*/ }
};

class ETFTransition {
 public :
   vector<int> proj;
   SDD value;

   ETFTransition (const vector<int> & proj) :  proj(proj),value(SDD::null) {};

   // returns the index of the variable in the projection
   // e.g. if transition touches vars 0, 3 , 5 and variable 3 is the target pid "var"
   // this will return 1 (i.e index of 3 in projection)
   // return -1 if var is not touched by the transition
   int concernsVariable (int var) const {
     for (vector<int>::const_iterator it = proj.begin() ; it != proj.end() ; ++it ) {
	if (*it == var) {
	  return var;
	}
     }
     return -1;
   }

   void addEdge (int *src, int * dst) {
     GSDD element = SDD::one;
     int len = proj.size();

     for (int i = 0; i < len; i++) {
	element = SDD(proj[i], DDD(DEFAULT_VAR,src[i],GDDD(DEFAULT_VAR,dst[i])) , element);
     }

     value = value + element;
     assert(value != SDD::top);
   }

   GShom getShom () const {
     return apply2k(value);
   }


 };



} // namespace

#endif /* ETFITS_HH_ */
