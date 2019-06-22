#ifndef __NET_TYPE__HH__
#define __NET_TYPE__HH__

#include <string>
#include <vector>
#include <list>
#include "its/Naming.hh"
#include "its/Ordering.hh"

#include "ddd/SDD.h"
#include "ddd/SHom.h"

namespace its {

/// a state or set of states (subsets of S) representation
typedef SDD State;
/// a transition representation ( successors(\tau) or locals() )
typedef Shom Transition;


/** The abstract type contract for Instantiable Transition Systems. */
/** Notations used are consistent with Tacas09 presentation  */
class Type {
public :
  virtual ~Type () {};

  virtual Label getName() const = 0;
  /** one initial state can be designated as initial state */
  virtual Label getDefaultState() const=0;
  /** the setter returns false if the label provided is not in InitStates */
  virtual bool setDefaultState (Label def) =0;
  
  /** the set InitStates of designated initial states (a copy)*/
  virtual labels_t getInitStates () const = 0;
  
  /** the set T of public transition labels (a copy)*/
  virtual labels_t getTransLabels () const = 0;
  
  /** state and transitions representation functions */
  /** Local transitions */
  virtual Transition getLocals () const = 0;
  
  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  virtual Transition getSuccs (const labels_t & tau) const = 0;
  
  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  virtual Transition getPredicate (char * predicate) const = 0;

  /** To obtain a representation of a labeled state */
  virtual State getState(Label stateLabel) const = 0;

  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  virtual State getPotentialStates(State reachable) const = 0;

  /** Allow to visit the underlying type definition */
  virtual void visit (class TypeVisitor * visitor) const = 0;

  /** Optional, ok to return true.
   * updates the variable order used in this type */
  virtual bool setVarOrder (labels_t vars) const = 0;
	
  // also sets to default if currently unset.
  virtual VarOrder * getVarOrder () const = 0;

  // collect the full list of qualified integer variables, in top to bottom fashion
  virtual void addFlatVarSet (labels_t & vars, Label prefix="") const =0;

  /** Optional, ok to do nothing.
   * prints the variable order used in this type */
  virtual void printVarOrder (std::ostream & os) const = 0;

  virtual std::ostream & print (std::ostream & os) const = 0;

  /** Print a set of states, explicitly. 
   *  The limit argument is used to print at most that many states. 
   *  Returns the number of states actually printed.
   */
  virtual long printState (State s, std::ostream & os, long limit=10) const = 0;


  typedef std::pair<vLabel,Transition> namedTr_t;
  typedef std::list<namedTr_t> namedTrs_t;
  typedef namedTrs_t::const_iterator namedTrs_it;
  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  virtual void getNamedLocals (namedTrs_t & ntrans) const = 0;

  typedef std::vector<size_t> varindex_t;
  /** Return the index of a given variable in the representation, actually a vector of indices in SDD case.
   */
  virtual void getVarIndex(varindex_t & index, Label vname) const =0;

  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  virtual Transition observe (labels_t obs, State potential) const = 0;
};

typedef const Type * pType;

} // namespace its

std::ostream & operator<< (std::ostream & os, const its::Type & t);

#endif

