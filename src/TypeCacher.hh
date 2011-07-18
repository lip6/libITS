#ifndef __STATE__CACHER__
#define __STATE__CACHER__

#include "Type.hh"

namespace its {

  /** A proxy type implementation of a cache for a Type implementation. */
  class TypeCacher : public Type {
    // pointer to concrete realization
    Type * concrete_;
    // a cache for state constructions
    typedef std::map<vLabel,State> state_cache_t;
    mutable state_cache_t state_cache;
    // a cache for succ constructions
    typedef std::vector<labels_t > tkey_t;
    mutable tkey_t trans_keys;
    mutable std::vector<Transition > trans_vals;
    // a cache for locals
    mutable Transition locals_;    
  public :
    // pass the concrete type and memory management
    TypeCacher (Type * concrete) : concrete_(concrete), locals_(GSDD::top) {};
    ~TypeCacher() { delete concrete_ ; }
    /** one initial state can be designated as initial state */
    Label getDefaultState() const { return concrete_->getDefaultState() ; }
    /** the setter returns false if the label provided is not in InitStates */
    bool setDefaultState (Label def) { return concrete_->setDefaultState(def) ; }
    /** the set InitStates of designated initial states */
    labels_t getInitStates () const { return concrete_->getInitStates() ; }
    
    /** the set T of public transition labels */
    labels_t getTransLabels () const { return concrete_->getTransLabels() ; }

    /** Print a set of states, explicitly. 
     *  Watch out, do not call on large its::State (>10^6). str is there for recursion, pass an empty string. */
    void printState (State s, std::ostream & os) const { return concrete_->printState(s,os); }

    /** The state predicate function : string p -> SHom.
     *  returns a selector homomorphism that selects states verifying the predicate 'p'.
     *  The syntax of the predicate is left to the concrete type realization.
     *  The only constraint is that the character '.' is used as a namespace separator
     *  and should not be used in the concrete predicate syntax.
     *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
    Transition getPredicate (Label predicate) const { return concrete_->getPredicate(predicate); }


    /** order */
    bool setVarOrder (labels_t vars) const { 
      state_cache.clear();
      trans_vals.clear();
      locals_ = Transition(GSDD::top);
      return concrete_->setVarOrder(vars); 
    }
    
    VarOrder * getVarOrder () const {
    	return concrete_->getVarOrder();
    }
    /** Optional, ok to return true.
     * updates the variable order used in this type */
    void printVarOrder (std::ostream & os) const { return concrete_->printVarOrder(os); }

    /** state and transitions representation functions */
    /** Cache is used . */
    /** Local transitions */
    Transition getLocals () const ;
    
    /** Successors synchronization function : Bag(T) -> SHom.
     * The collection represented by the iterator should be a multiset
     * of transition labels of this type (as obtained through getTransLabels()).
     * Otherwise, an assertion violation will be raised !!
     * Cache is used
     * */
    Transition getSuccs (const labels_t & tau) const ;
    
    /** To obtain a representation of a labeled state */
    /** Cache is used */
    State getState(Label stateLabel) const;
    
    Label getName() const { return concrete_->getName() ; }
    std::ostream & print ( std::ostream& os ) const { return concrete_->print(os);}
    
    void visit (class TypeVisitor * visitor) const { concrete_->visit(visitor); }

    void getNamedLocals (namedTrs_t & ntrans) const { concrete_->getNamedLocals(ntrans); }

    /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
     *  Uses the provided "reachable" states to compute the variable domains. */
    State getPotentialStates(State reachable) const { return concrete_->getPotentialStates(reachable); }

    /** Return a Transition that maps states to their observation class.
     *  Observation class is based on the provided set of observed variables, 
     *  in standard "." separated qualified variable names. 
     *  The returned Transition replaces the values of non-observed variables
     *  by their domain.
     **/ 
    Transition observe (labels_t obs, State potential) const {
      return  concrete_->observe(obs, potential);
    }


  };

}

#endif
