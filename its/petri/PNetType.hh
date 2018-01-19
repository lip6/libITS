#ifndef __LABELED_NET_HH__
#define __LABELED_NET_HH__

#include "its/TypeBasics.hh"
#include "its/petri/PNet.hh"
#include "its/petri/TPNetSemantics.hpp"
#include "its/TypeVisitor.hh"
#include <iosfwd>
#include <cstdlib>
#include <cstdio>
#include <map>


namespace its {


/** Implement a type by adapting the PNet interface.
 *  Using delegation to build a type from a PNet */
template < typename  HomType >
class PNetTType :  public TypeBasics {
 protected :
  // the concrete storage class
  PNet net_;
  // used to label SDD node of PNet.
  static const int DEFAULT_VAR = 0;

  virtual labels_t getVarSet () const {
    labels_t pnames ;
    for (PNet::places_it it = net_.places_begin() ; it != net_.places_end(); ++it ) {
      pnames.push_back(it->getName());
    }
    labels_t pnames2;
//     for (labels_t::const_reverse_iterator rit = pnames.rbegin() ; rit != (const_cast<const labels_t &> (pnames)).rend() ; ++rit ) {
//       pnames2.push_back(*rit);
//     }
    return pnames;
  }

  // return the reset transition to reset disbled transition clocks
  virtual Transition getResetDisabled () const {
    return Transition::id;
  }

public :

  typedef  TPNetSemantics<HomType> Semantics ;

  PNetTType (const PNet & net) : net_(net) {};

  /** the set InitStates of designated initial states */
  labels_t getInitStates () const {
    labels_t labs;
    for (PNet::markings_it it = net_.markings_begin(); it != net_.markings_end() ; ++it) {
      labs.push_back(it->first);
    }
    return labs;
  }

  /** the set T of public transition labels */
  labels_t getTransLabels () const {
    labels_t labs;
    for ( PNet::trans_it it = net_.transitions_begin() ; it != net_.transitions_end(); ++it ) {
      if (it->getVisibility() == PUBLIC)
	labs.push_back(it->getLabel());
    }
    return labs;
  }

  virtual Transition getTransitionHom (const PTransition & t, const VarOrder & vo) const {
    return Semantics::encapsulate( Semantics::getFullHom(t, vo)) ;
  }

  /** state and transitions representation functions */
  /** Local transitions */
  Transition getLocals () const {
    std::set<GShom> locals;
    const VarOrder & vo = *getVarOrder();
    for ( PNet::trans_it it = net_.transitions_begin() ; it != net_.transitions_end(); ++it ) {
      if (it->getVisibility() == PRIVATE) {
	Transition ht = getTransitionHom (*it,vo);
	if (! ht.is_selector()) 
	  locals.insert( getResetDisabled() & ht );
	else
	  locals.insert(ht);
      }
    }
    if (! locals.empty())
      return GShom::add(locals);
    else
      return GShom(GSDD::null);
  }

  void getNamedLocals (namedTrs_t & locals) const {
    const VarOrder & vo = *getVarOrder();
    for ( PNet::trans_it it = net_.transitions_begin() ; it != net_.transitions_end(); ++it ) {
      if (it->getVisibility() == PRIVATE) {
	Transition ht = getTransitionHom (*it,vo);
	if (! ht.is_selector()) 
	  ht = getResetDisabled() & ht ;
	locals.push_back(namedTr_t(it->getName(),ht));
      }
    }
  }


  virtual HomType getTransitionEnabler (const PTransition & t, const VarOrder & vo) const {
    return  Semantics::getEnablerHom(t, vo) ;
  }

  virtual HomType getTransitionAction (const PTransition & t, const VarOrder & vo) const {
    return  Semantics::getActionHom(t, vo) ;
  }
    

  
  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  virtual Transition getSuccs (const labels_t & tau) const {
    Transition resultTrans = Transition::id;
    // use the provided order
    const VarOrder * po =  getVarOrder();
    // iterate on labels
    for (labels_t::const_iterator it = tau.begin() ; it != tau.end() ; ++it) {
    	Transition  labelAction;
    	bool isFirstTrWithLabel = true;
        for (PNet::trans_it t = net_.transitions_begin(); t != net_.transitions_end(); ++t ) {
        	if (t->getLabel() == *it) {
        		if (! isFirstTrWithLabel) {
        			labelAction = labelAction +  getTransitionHom(*t, *po) ;
        		} else {
        			// add the effect of this action
        			labelAction = getTransitionHom(*t, *po) ;
        			isFirstTrWithLabel = false;
        		}
        	}
        }
        if (isFirstTrWithLabel) {
        	std::cerr << "Asked for succ by transition "<< *it << " but no such transition label exists in type " << net_.getName() << std::endl ;
        	assert(false);
        }
        resultTrans = labelAction & resultTrans ;
    }
    // The DDD state variant uses an intermediate variable called DEFAULT_VAR
    return resultTrans;
  }
	
  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition getAPredicate (Label predicate) const {
    AtomicPredicate pred = parseAtomicPredicate(predicate);


//      std::cerr << "Petri net parsed predicate var:" << var << " comp:" << comp << " value:"<<val <<std::endl;
//      std::cerr << "Translates to hom :" << Semantics::getHom ( foo, index, value) << std::endl;

    return Semantics::encapsulate( Semantics::getHom ( pred.comp , pred.var , pred.val) );
  }

  /** To obtain a representation of a labeled state */
  State getPotentialStates(State reachable) const {
    return Semantics::getPotentialStates (reachable, *getVarOrder());
  }


  /** To obtain a representation of a labeled state */
  State getState(Label stateLabel) const {
    PNet::markings_it it = net_.markings_find(stateLabel);
    if (it == net_.markings_end()) {
      std::cerr << "Error while reading model : unknown state label :\"" << stateLabel << "\" in type " << getName()<< std::endl;
      std::cerr << "State labels defined are : ";
      for (PNet::markings_it it = net_.markings_begin() ; it != net_.markings_end() ; /* in loop */ ) {
	std::cerr << it->first ;
	++it;
	if (it != net_.markings_end() ) {
	  std::cerr << ", ";
	} else {
	  std::cerr << std::endl;
	  break;
	}
      }
      std::cerr << "Error is fatal, sorry."  << std::endl;
      // DO NOT MISUSE : the state must have been defined
      assert (false) ;
      // for compiler warning : no return
      return State::null;
    } else {
      // an assignment style view of a marking
      const Marking & m = it->second;
      const VarOrder * vo = getVarOrder();
      
      return Semantics::getState (m, *vo);
    }
  }

  void getVarIndex(varindex_t & index, Label vname) const {
     int target = getVarOrder()->getIndex(vname);
     Semantics::getVarIndex(index,target);
  }

  /** Use delegation on net_ */
  Label getName() const {
    return net_.getName();
  }

  std::ostream& print(std::ostream& os) const {
    return net_.print(os);
  }

  /** Print a set of states, explicitly. 
   *  Watch out, do not call on large its::State (>10^6) */
  long printState (State s, std::ostream & os, long limit) const {
    return Semantics::printState (s, os, limit, *getVarOrder());
  }

  /** Allow to visit the underlying type definition */
  virtual void visit (class TypeVisitor * visitor) const {
    visitor->visitPNet(net_);
  }
  
  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition observe (labels_t obs, State potential) const {
    return Semantics::observe (obs, potential, *getVarOrder());
  }


};


  typedef PNetTType<GHom> PNetType;
  typedef PNetTType<GShom> PNetSType;


}

template<typename HomType>
std::ostream & operator<< (std::ostream & os, const its::PNetTType<HomType> & net) {
  return net.print(os);
};


#endif
