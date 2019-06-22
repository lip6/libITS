#ifndef __TYPE_DECORATOR__
#define __TYPE_DECORATOR__

#include "its/Type.hh"

namespace its {

  /** A proxy type implementation of a cache for a Type implementation. */
  class TypeDecorator : public Type {
    // pointer to concrete realization
    pType concrete_;
   public :
    // pass the concrete type and memory management
    TypeDecorator (pType concrete) : concrete_(concrete) {};
    virtual ~TypeDecorator() { delete concrete_ ; }
    /** one initial state can be designated as initial state */
    virtual Label getDefaultState() const { return concrete_->getDefaultState() ; }
    /** the setter returns false if the label provided is not in InitStates */
    virtual bool setDefaultState (Label def) { return ((Type *)concrete_)->setDefaultState(def) ; }
    /** the set InitStates of designated initial states */
    virtual labels_t getInitStates () const { return concrete_->getInitStates() ; }
    
    /** the set T of public transition labels */
    virtual labels_t getTransLabels () const { return concrete_->getTransLabels() ; }

    /** Print a set of states, explicitly. 
     *  Watch out, do not call on large its::State (>10^6). str is there for recursion, pass an empty string. */
    virtual long printState (State s, std::ostream & os, long limit) const { return concrete_->printState(s,os,limit); }

    /** The state predicate function : string p -> SHom.
     *  returns a selector homomorphism that selects states verifying the predicate 'p'.
     *  The syntax of the predicate is left to the concrete type realization.
     *  The only constraint is that the character '.' is used as a namespace separator
     *  and should not be used in the concrete predicate syntax.
     *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
    virtual Transition getPredicate (char* predicate) const { return concrete_->getPredicate(predicate); }

    void addFlatVarSet (labels_t & vars, Label prefix) const { concrete_->addFlatVarSet(vars, prefix); }
    /** order */
    virtual bool setVarOrder (labels_t vars) const { 
      return concrete_->setVarOrder(vars); 
    }
    
    virtual VarOrder * getVarOrder () const {
    	return concrete_->getVarOrder();
    }
    /** Optional, ok to return true.
     * updates the variable order used in this type */
    virtual void printVarOrder (std::ostream & os) const { return concrete_->printVarOrder(os); }

    /** state and transitions representation functions */
    /** Cache is used . */
    /** Local transitions */
      virtual Transition getLocals () const {
          return concrete_->getLocals();
      }
    
    /** Successors synchronization function : Bag(T) -> SHom.
     * The collection represented by the iterator should be a multiset
     * of transition labels of this type (as obtained through getTransLabels()).
     * Otherwise, an assertion violation will be raised !!
     * Cache is used
     * */
      virtual Transition getSuccs (const labels_t & tau) const {
          return concrete_->getSuccs(tau);
      }
    
    /** To obtain a representation of a labeled state */
      virtual State getState(Label stateLabel) const {
          return concrete_->getState(stateLabel);
      }
    
    virtual Label getName() const { return concrete_->getName() ; }
    virtual std::ostream & print ( std::ostream& os ) const { return concrete_->print(os);}
    
    virtual void visit (class TypeVisitor * visitor) const { concrete_->visit(visitor); }

    virtual void getNamedLocals (namedTrs_t & ntrans) const { concrete_->getNamedLocals(ntrans); }

    /** Return the index of a given variable in the representation, actually a vector of indices in SDD case.
     */
    virtual void getVarIndex(varindex_t & index, Label vname) const {
      concrete_->getVarIndex (index,vname);
    }


    /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
     *  Uses the provided "reachable" states to compute the variable domains. */
    virtual State getPotentialStates(State reachable) const { return concrete_->getPotentialStates(reachable); }

    /** Return a Transition that maps states to their observation class.
     *  Observation class is based on the provided set of observed variables, 
     *  in standard "." separated qualified variable names. 
     *  The returned Transition replaces the values of non-observed variables
     *  by their domain.
     **/ 
    virtual Transition observe (labels_t obs, State potential) const {
      return  concrete_->observe(obs, potential);
    }


  };

}

#endif
