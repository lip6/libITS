/*
 * ITSModel.hh
 *
 *  Created on: 3 d�c. 2008
 *      Author: yann
 */

#ifndef ITSMODEL_HH_
#define ITSMODEL_HH_

// an include from libddd for hash_map
#include "util/configuration.hh"

#include "Type.hh"
#include "Instance.hh"
#include "Ordering.hh"

#include <vector>
#include <set>
#include <iosfwd>


/** predeclaration to avoid dependency on spot */
namespace spot {
  class tgba ;
}
namespace json {
  class Hierarchie;
}

namespace its {

  // used to parametrize construction
  enum storage { ddd_storage, sdd_storage };
  // representation strategies
  enum scalarStrategy {DEPTH1, SHALLOWREC, RECFOLD, DEPTHREC, DEPTHPROG};


class ITSModel {
  // To store a set of type declarations
  typedef std::vector<pType> types_t;
  typedef types_t::const_iterator types_it;
protected:
  // The known type declarations
  types_t types_;
  // The model main instance (may be NULL)
  pInstance model_;
private:
  // Reachable state space
  mutable State reached_;
  // Predecessor relation
  mutable Transition predRel_;
  // initial state(s) label
  vLabel initName_;
  // for memory management
  std::set<size_t> dontdelete;
  // the current storage strategy.
  storage storage_;
  scalarStrategy scalarStrat_;
  int scalarParam_;

  /** return the time elapse transition or Transition::id if untimed model. */
  Transition getElapse () const ;

protected :
  // add a type to the type declarations
  // returns false if the type name already exists
  virtual bool addType (pType type);
  
public :
  // default constructor
  ITSModel () : model_(NULL),reached_(State::null),predRel_(Transition::null),storage_(sdd_storage), scalarStrat_(DEPTH1), scalarParam_(1) {};
  // quite a bit of cleanup necessary given the use of pointers...
  ~ITSModel () ;

  // Creates an instance of the given "type" of name "name" and sets it as the main instance of the model.
  // Returns false if the type name does not exist
  virtual bool setInstance (Label type, Label name);
  // defines the initial state of the main instance, should be a state label of type(main)
  // returns false in case of failure (no main instance defined, no corresponding state label)
  bool setInstanceState (Label stateName);
  // returns a const pointer to a type given its name
  pType findType (Label name) const ;
  // returns a const pointer to the main instance
  pInstance getInstance () const ;

  // returns the SDD version of the initial state
  State getInitialState () const ;

  // Visitor pattern to work on the underlying types
  void visitTypes (class TypeVisitor *) const ;

  // Play factory role for building ITS types from other formalisms
  // Returns false and aborts if type name already exists.
  // Create a type to hold a Petri net.
  virtual bool declareType (const class PNet & net);
  // Create a type to hold a Petri net, with hierarchical representation based on JSON description.
  bool declareType (const class PNet & net, const class json::Hierarchie * hier);
  // Create a type to hold a Timed Petri net.
  bool declareType (const class TPNet & net);
  // Create a type to hold a composite
  bool declareType (const class Composite & comp);
  // Create a type to hold a timed composite
  bool declareType (const class TComposite & comp);
  // Create a type to hold a scalar set
  bool declareType (const class ScalarSet & net) ;
  // Create a type to hold a circular set
  bool declareType (const class CircularSet & net) ;
  // Create a type to hold a spot::tgba : this function is implemented in sog-its project, file sogIts.cpp
  bool declareType (const class spot::tgba & tgba);
  // Create a type to represent a PINS wrapper on the ETF file format.
  bool declareETFType (Label path) ;

  // Set the behavior for TPN factory from SDD to DDD strategy.
  // Affects subsequent call to declareType.
  void setStorage (const storage & s) { storage_ = s; }
  // Set the behavior for Scalar set strategy.
  // Affects subsequent call to declareType.
  void setScalarStrategy (scalarStrategy s, int param=1) { scalarStrat_ = s ; scalarParam_ =param ; }

  // allow to manually define an order for a type
  // !! no controls, if var set is incomplete errors will occur
  bool updateVarOrder (Label type, const labels_t & order);
  void printVarOrder (std::ostream & os) const;
  bool loadOrder (std::istream & is);

  // returns the "Next" relation, i.e. exactly one step of the transition relation.
  // tests for presence of "elapse" transition.
  Transition getNextRel () const ;
  // returns the predecessor relationship, i.e. exactly one step backward of the transition relation.
  // This function uses the NextRel to compute the reverse transition relation.
  // If reach is left to its default value "null", all reachable states are used as envelope.
  // Else the the transition relation is constrained to stay within "reach".
  Transition getPredRel (State reach=State::null) const ;
  // returns the full reachable state space of the system from the initial state(s)
  // also caches result. Optional parameter to deactivate garbage collection.
  State computeReachable (bool wGarbage=true) const;
  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition getPredicate (Label predicate) const { return getInstance()->getType()->getPredicate(predicate); }
  
  /** Returns a shortest witness trace expressed in transition names leading from a state of init to a state in final. */
  labels_t findPath (State init, State toreach, State reach) const;  

  // semi private function used in Scalar sandboxes
  void cloneType (pType type) { int n = types_.size(); types_.push_back(type); dontdelete.insert(n) ; }
  // member pretty print
  void print (std::ostream & os) const ;
};

} // namespace

// convenience call to member print
std::ostream & operator << (std::ostream & os, const its::ITSModel & m);

#endif /* ITSMODEL_HH_ */
