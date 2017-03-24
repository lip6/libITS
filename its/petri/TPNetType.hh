#ifndef __TPNET_TYPE_HH__
#define __TPNET_TYPE_HH__

#include "its/petri/TPNetType.hh"
#include "its/petri/TPNet.hh"

namespace its {
/** Implement a type by adapting the TPNet interface.
 *  Using delegation to build a type from a TPNet 
 *  This version uses DDD storage with one external SDD variable.
 *  See TPNetSType for native SDD storage version.
 */
  template < typename  HomType >
  class TPNetTType : public PNetTType<HomType> {
  protected :
    // returns the set of variable names used in this encoding
    labels_t getVarSet () const {
      // get list of places from the Petri net
      labels_t pnames =  PNetTType<HomType>::getVarSet();
      // Add a clock for each timed transition
      labels_t cnames ;
      for (PNet::trans_it it = this->net_.transitions_begin() ; it != this->net_.transitions_end(); ++it ) {
	if (it->isTimed() && it->getClock().lft!=0)
	  cnames.push_back(TPNet::clockName(*it));
      }
      cnames.insert(cnames.end(),pnames.begin(),pnames.end());
      return cnames;
    }

    // return the reset transition to reset disbled transition clocks
    Transition getResetDisabled () const {
	const VarOrder * vo = this->getVarOrder();
	HomType elapse = HomType::id;
 	for ( TPNet::trans_it it = this->net_.transitions_begin() ; it != this->net_.transitions_end(); ++it ) {
 	  // now consider local enablign conditions for public transitions
 	  if (true || it->getVisibility() == PRIVATE) {
 	    if ( it->isTimed() && it->getClock().lft!=0) {	      
	      int cvar = vo->getIndex( TPNet::clockName(*it) );
 	      // Case 2 : Time step
 	      // Case 2.1 : t disabled, do a reset
 	      // Case 2.2 : t enabled, max bound not reached, increment
 	      HomType timet = ITE (Semantics::getEnablerHom(*it,*vo), 
				   HomType::id,
				   Semantics::getRZ(it->getClock(), cvar) );
 	      // Add this to the global elapse synchronization
 	      elapse = elapse & timet;
 	    }
 	  }
 	}
	return Semantics::encapsulate(elapse);


    }
 public :
    typedef typename TPNetTType<HomType>::Semantics Semantics;

    TPNetTType (const TPNet & net) : PNetTType<HomType> (net) {};
    
    // also reset !
    Transition getTransitionHom (const PTransition & t, const VarOrder & vo) const {
      if ( ! t.isTimed() ) {
	// default back to untimed behavior
	return PNetTType<HomType>::getTransitionHom(t,vo);
      } else {
	// The real TPN stuff
	// Case 1 : transition fires 
	// full(t) = c_t.RZ & c_t.OK & action(t) & enabler(t)
	int cvar = vo.getIndex( TPNet::clockName(t) );
	const TimeConstraint & tc = t.getClock();
	return Semantics::encapsulate( Semantics::getFullHom(t, vo)
				       & Semantics::getRZ (tc,cvar)
					 & Semantics::getOK (tc,cvar) ) ;
      }       
    }

      
    virtual HomType getTransitionEnabler (const PTransition & t, const VarOrder & vo) const {
      if ( t.isTimed() ) {
	int cvar = vo.getIndex( TPNet::clockName(t) );
	return Semantics::getEnablerHom(t, vo) & Semantics::getOK(t.getClock(),cvar) ;
      } else {
	return PNetTType<HomType>::getTransitionEnabler(t,vo);
      }
    }
    
    virtual HomType getTransitionAction (const PTransition & t, const VarOrder & vo) const {
      if ( t.isTimed() ) {
	int cvar = vo.getIndex( TPNet::clockName(t) );
	return Semantics::getActionHom(t, vo) & Semantics::getRZ(t.getClock() ,cvar)  ;
      } else {
	return PNetTType<HomType>::getTransitionAction(t,vo);
      }
    }

    
    Transition getSuccs (const labels_t & tau) const {
      labels_t eltab;
      eltab.push_back("elapse");
      if (tau == eltab) {
	const VarOrder * vo = this->getVarOrder();
	HomType elapse = HomType::id;
 	for ( TPNet::trans_it it = this->net_.transitions_begin() ; it != this->net_.transitions_end(); ++it ) {
 	  // now consider local enablign conditions for public transitions
 	  if (true || it->getVisibility() == PRIVATE) {
 	    if (it->getClock().lft == 0) {
	      HomType timet = ITE (Semantics::getEnablerHom(*it,*vo), 
				   Semantics::getIncr (it->getClock(), 0),
				   HomType::id );
	      // Add this to the global elapse synchronization
 	      elapse = elapse & timet;
 
// 	      std::cerr << "Added " << it->getName() << " " << timet << std::endl;
// 	      std::cerr << "Obtained " << elapse << std::endl;
	    } else if ( it->isTimed() ) {	      
	      int cvar = vo->getIndex( TPNet::clockName(*it) );
 	      // Case 2 : Time step
 	      // Case 2.1 : t disabled, do a reset
 	      // Case 2.2 : t enabled, max bound not reached, increment
 	      HomType timet = ITE (Semantics::getEnablerHom(*it,*vo), 
				Semantics::getIncr (it->getClock(), cvar),
				Semantics::getRZ(it->getClock(), cvar) );
 	      // Add this to the global elapse synchronization
 	      elapse = elapse & timet;
 	    }
 	  }
 	}
	return Semantics::encapsulate(elapse);
      } else {
	Transition ret = PNetTType<HomType>::getSuccs(tau);
	if (! ret.is_selector() ) {
	  ret = getResetDisabled() & ret;
	}
	return ret;
      }
    }

};


    typedef TPNetTType<GHom> TPNetType;
    typedef TPNetTType<GShom> TPNetSType;

}

std::ostream & operator<< (std::ostream & os, const its::TPNetType & net);


#endif
