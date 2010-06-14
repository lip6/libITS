#ifndef __NET_TYPE__HH__
#define __NET_TYPE__HH__

#include <string>
#include <vector>
#include "Naming.hh"

#include "SDD.h"
#include "SHom.h"

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
  virtual Transition getPredicate (Label predicate) const = 0;

	/** To obtain a representation of a labeled state */
    virtual State getState(Label stateLabel) const = 0;

  /** Allow to visit the underlying type definition */
  virtual void visit (class TypeVisitor * visitor) const = 0;

  /** Optional, ok to return true.
   * updates the variable order used in this type */
  virtual bool setVarOrder (labels_t vars) const = 0;

  /** Optional, ok to do nothing.
   * prints the variable order used in this type */
  virtual void printVarOrder (std::ostream & os) const = 0;

  virtual std::ostream & print (std::ostream & os) const = 0;

  /** Print a set of states, explicitly. 
   *  Watch out, do not call on large its::State (>10^6). */
  virtual void printState (State s, std::ostream & os) const = 0;

};

typedef const Type * pType;

} // namespace its

std::ostream & operator<< (std::ostream & os, const its::Type & t);

#endif

