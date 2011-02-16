#ifndef __LABELED_NET_HH__
#define __LABELED_NET_HH__

#include "TypeBasics.hh"
#include "PNet.hh"
#include "TPNetSemantics.hpp"
#include "TypeVisitor.hh"
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
    HomType resultTrans = HomType::id;
    // use the provided order
    const VarOrder * po =  getVarOrder();
    // iterate on labels
    for (labels_t::const_iterator it = tau.begin() ; it != tau.end() ; ++it) {
    	HomType  labelAction;
    	bool isFirstTrWithLabel = true;
        for (PNet::trans_it t = net_.transitions_begin(); t != net_.transitions_end(); ++t ) {
        	if (t->getLabel() == *it) {
        		if (! isFirstTrWithLabel) {
        			labelAction = labelAction +  Semantics::getFullHom(*t, *po) ;
        		} else {
        			// add the effect of this action
        			labelAction = Semantics::getFullHom(*t, *po) ;
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
    Transition res = Semantics::encapsulate(resultTrans);
    return res;
  }
	
  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition getPredicate (Label predicate) const {
    // The predicate should respect the grammar : varName (=|>|<|>=|<=|<|!=) value
    // Where varName is a place or a transition (hence designating its clock) such as found in getVariableSet(), getVarOrder()
    // and value is an integer
    
    // step 1 : parse the predicate
    const char * pred = predicate.c_str();
    int mode = 0;
    vLabel var,comp,val;
    for (const char * cp = pred ; *cp ; ++cp) {
      // skip ws
      if (*cp == ' ')
	continue;
      switch (mode) {
	// parsing variable
      case 0:
	if (*cp == '!' || *cp == '>' || *cp == '=' || *cp == '<') {
	  mode = 1;
	  comp += *cp;
	} else {
	  var += *cp;
	}
	break;
      case 1:
	if (*cp == '!' || *cp == '>' || *cp == '=' || *cp == '<') {
	  comp += *cp;
	} else {
	  val += *cp;
	}
	mode = 2;
	break;
      case 2:
	val += *cp;
      }
    }
    // Check parse
    int index = getVarOrder()->getIndex(var);
    if ( index == -1 ) {
      std::cerr << "Error variable " + var + " cannot be resolved when trying to parse predicate : "  + predicate << std::endl;
      std::cerr << "Failing with error code 2"<< std::endl;
      exit (2);
    }
    GHom (* foo) (int,int)  = NULL;
    if ( comp == "=" ) {
      foo = & varEqState;
    } else if (comp == "!=") {
      foo = & varNeqState;
    } else if (comp == ">") {
      foo = & varGtState;
    } else if (comp == "<") {
      foo = & varLtState;
    } else if (comp == "<=") {
      foo = & varLeqState;
    } else if (comp == ">=") {
      foo = & varGeqState;
    } else {
      std::cerr << "Unrecognized comparison operator : " << comp << " when parsing predicate " << predicate << std::endl;
      std::cerr << "Error is fatal, failing with error code 2" << std::endl;
      exit (2);
    }
    int value = 0;
    if ( sscanf (val.c_str(), "%d" , &value) == 0 ) {
      std::cerr << "Unable to parse right hand side of comparison as an integer : " << comp << " when parsing predicate " << predicate << std::endl;
      std::cerr << "Error is fatal, failing with error code 2" << std::endl;
      exit (2);      
    }

//      std::cerr << "Petri net parsed predicate var:" << var << " comp:" << comp << " value:"<<val <<std::endl;
//      std::cerr << "Translates to hom :" << Semantics::getHom ( foo, index, value) << std::endl;

    return Semantics::encapsulate( Semantics::getHom ( foo, index, value) );
  }



  /** To obtain a representation of a labeled state */
  State getState(Label stateLabel) const {
    PNet::markings_it it = net_.markings_find(stateLabel);
    if (it == net_.markings_end()) {
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

  /** Use delegation on net_ */
  Label getName() const {
    return net_.getName();
  }

  std::ostream& print(std::ostream& os) const {
    return net_.print(os);
  }

  /** Print a set of states, explicitly. 
   *  Watch out, do not call on large its::State (>10^6) */
  void printState (State s, std::ostream & os) const {
    Semantics::printState (s, os, *getVarOrder());
  }

  /** Allow to visit the underlying type definition */
  virtual void visit (class TypeVisitor * visitor) const {
    visitor->visitPNet(net_);
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
