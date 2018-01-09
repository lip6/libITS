#ifndef __CTL__CHECK__HH__
#define __CTL__CHECK__HH__


#include "its/ITSModel.hh"
#include "ctlp/ctlp.h"


class CTLChecker {


  const its::ITSModel & model;

  /** A cache for homomorphism construction of AP */
  typedef std::map<vLabel, its::Transition> hom_map_t;
  typedef hom_map_t::const_iterator hom_map_it;
  mutable hom_map_t hom_map;

  /** A cache containing the Homomorphisms representing formulas */
  typedef std::map<Ctlp_Formula_t *, its::Transition> ctl_cache_t;
  typedef ctl_cache_t::const_iterator ctl_cache_it;
  mutable ctl_cache_t ctl_cache;
  /** A cache containing the states representing formulas */
  typedef std::map<Ctlp_Formula_t *, its::State> ctl_statecache_t;
  typedef ctl_statecache_t::const_iterator ctl_statecache_it;
  mutable ctl_statecache_t ctl_statecache;

  mutable its::Transition predRel_;

  bool isfairtime;
  bool beQuiet;
  mutable bool scc_;
  mutable bool scc_computed_;
public :
  CTLChecker (const its::ITSModel & model, bool beQuiet=false) : model(model), isfairtime(false), beQuiet(beQuiet), scc_(false), scc_computed_(false) {}

  void setFairTime(bool befairtime) { isfairtime = befairtime; }

  its::Transition getSelectorAP (Label apname, Label value) const;
  its::Transition getAtomicPredicate (Label ap) const;

  its::Transition getHomomorphism (Ctlp_Formula_t *formula) const;

  its::State getStateVerifying (Ctlp_Formula_t *formula, bool need_exact=true) const;

  its::Transition getPredRel (its::State envelope=its::State::null) const ;

  its::Transition getNextRel () const ;

  its::State getReachable () const;

  bool hasSCCs () const;

  its::State getInitialState () const;

  its::State getReachableDeadlocks () const;
  its::State getReachableTimelocks () const;

  /** Returns a shortest witness trace expressed in transition names leading from a state of init to a state in final. */
  its::path_t findPath (its::State init, its::State toreach, its::State reach, bool precise = false) const { return model.findPath(init,toreach,reach, precise); }

  /** Prints a path. The printing invokes the main instance's type's printing mechanism.
   ** The limit is used to avoid excessive sizes of output : only the first "limit" states (or an approximation thereof in SDD context) are shown. 
   ** The boolean "withStates" controls if only transitions are shown or states as well 
   **/
  void printPath (const its::path_t &path, std::ostream & out, bool withStates=false) {  model.printPath(path, out, withStates) ; }

  void printSomeStates (its::State states, std::ostream & out) const {
    model.printSomeStates(states,out);
  }

  // Explain the truth value of formula in given states.
  // Formula is true if at least some input states satify it.
  // Writes to out a diagnosis.
  // Returns states that were actually explained, a subset of the input states.
  its::State explain (its::State sat, Ctlp_Formula_t *ctlFormula, std::ostream & out) const ;


  labels_t buildWitness (Ctlp_Formula_t *formula) const;
};



#endif
