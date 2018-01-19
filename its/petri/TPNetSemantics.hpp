#ifndef __PTRANSITION__SEMANTICS__
#define __PTRANSITION__SEMANTICS__

#include "ddd/Hom.h"
#include "ddd/SHom.h"

#include "ddd/Hom_Basic.hh"
#include "its/petri/Hom_PlaceArcs.hh"
#include "its/petri/PTransition.hh"
#include "its/Observe_Hom.hh"

#include <algorithm>

namespace its {

  template <typename HomType> 
  class TPNetSemantics {
  public :
    typedef typename HomType::NodeType NodeType;
  
    // triggers foo(var,val) or localApply( foo(DEFAULT, val) , var) according to 
    // partial specialization used (see bottom of this file)
    static HomType getHom ( GHom (* foo) (int,int) , int var, int val) ;
  
    static HomType getEnablerHom (const Arc & a, const VarOrder & pi) {
      // to handle hyper arcs, we use a set and OR the result
      std::set<HomType> homs;
      if ( a.getType() == OUTPUT || a.getType() == RESET )
	return HomType::id;
      for (Arc::places_it it = a.begin(); it != a.end() ; ++it) {
	int val = it->getValuation();
	int pindex = pi.getIndex(it->getPlace());
	switch (a.getType()) {
	case INHIBITOR :
	  homs.insert( getHom ( varLeqState , pindex, val-1 ) );
	  break;
	case INPUT :
	case  TEST :
	  homs.insert( getHom ( varGtState , pindex, val-1 ) ); 
	  break;
	case OUTPUT :
	case RESET :
	  // no action needs to be taken
	  break;
	}
      }
      return HomType::add(homs);
    }
  
    static HomType getDisablerHom (const Arc & a, const VarOrder & pi) {
      return ! getEnablerHom(a,pi);
    }
  
    static HomType getActionHom (const Arc & a, const VarOrder & pi) {
      // to handle hyper arcs, we use a set and OR the result
      std::set<HomType> homs;
      if (a.getType() == INHIBITOR || a.getType() == TEST)
	return HomType::id;
      for (Arc::places_it it = a.begin(); it != a.end() ; ++it) {
	int val = it->getValuation();
	int pindex = pi.getIndex(it->getPlace());
	switch (a.getType()) {
	case INPUT :
	  // negative post arc ! no enabling test-set behavior
	  homs.insert( getHom ( postPlace, pindex, -val ));
	  break;
	case OUTPUT :
	  // the varGtState is introduced to help the invert relation be exact
	  homs.insert( getHom ( varGtState , pindex, 0 ) & getHom ( postPlace, pindex, val )); 
	  break;
	case RESET :
	  homs.insert( getHom ( setVarConst, pindex, 0) ); 
	  break;
	case TEST :
	case INHIBITOR :
	  // no action needs to be taken
	  return HomType::id;
	  break;
	}
      }
      return HomType::add(homs);
    }



    static HomType getActionHom (const PTransition & t, const VarOrder & pi) {
      HomType res;
      for (PTransition::arcs_it it = t.begin() ; it != t.end() ; ++it ) {
	res = getActionHom(*it,pi) & res ;
      }
      return res;
    }
    
    static HomType getEnablerHom (const PTransition & t, const VarOrder & pi) {
      HomType res;
      for (PTransition::arcs_it it = t.begin() ; it != t.end() ; ++it ) {
	res = getEnablerHom(*it,pi) & res ;
      }
      return res;
    }

    static HomType getFullHom (const PTransition & t, const VarOrder & pi) {  
      return getActionHom(t,pi) & getEnablerHom(t,pi);
    }

    static HomType getOK (const TimeConstraint & clock, int cvar) {
      HomType h = HomType::id;
      if ( clock.lft == 0) {
	// [0,0] clocks are always ok ...
	return HomType::id;
      }
      if ( clock.lft != -1 )
	// not an infinite lft
	h = getHom ( varLeqState , cvar, clock.lft ) ;
      if ( clock.eft > 0 ) 
	// not earliest firing time = 0
	h = getHom ( varGtState, cvar, clock.eft - 1) & h;
      return h;
    }

    static HomType getRZ (const TimeConstraint & clock, int cvar)  {
      if (clock.lft == 0)
	// [0,0] clocks are never incremented... no need to reset
	return HomType::id;
      return getHom ( setVarConst, cvar, 0 ) ;
    }    

    static HomType getIncr (const TimeConstraint & clock, int cvar)  {
      if ( clock.lft == -1 ) {
	if (clock.eft == 0) {
	  // [0, INF] clock
	  return HomType::id;
	} else {
	  // [eft,INF] case
	  // if lft = INF, only increment up to eft, then leave at value
	  return  ITE( getHom(varGtState, cvar, clock.eft-1), HomType::id,  getHom( postPlace,cvar,1)) ;
	}
      } else if (clock.lft == 0) {
	// [0,0] bounds
	return NodeType::null;
      }
      // [x, y] general case, x >= 0 and y < INF
      return getHom(postPlace,cvar,1) & getHom(varLeqState, cvar, clock.lft - 1);
    }
    
    static NodeType newNode (int var, int val) ;

    static void getVarIndex (Type::varindex_t & index, int target) ;


    static NodeType getMarking (const Marking &m, const VarOrder & vo) {
      // converting to DDD first
      NodeType M0 = NodeType::one;
      // each place = one var as indicated by getPorder
      for (size_t i=0 ; i < vo.size() ; ++i) {
	Label pname = vo.getLabel(i);
	// retrieve the appropriate place marking
	int mark = m.getMarking(pname);
	// left concatenate to M0
	M0 = newNode (i,mark) ^ M0;
	// for pretty print
	DDD::varName(i,pname);
      }
      return M0; 
    }
    
    static GSDD getState (const Marking &m, const VarOrder & vo) ;

    /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
     *  Uses the provided "reachable" states to compute the variable domains. */
    static State getPotentialStates (State reachable, const VarOrder & vo) ;

    /** Return a Transition that maps states to their observation class.
     *  Observation class is based on the provided set of observed variables, 
     *  in standard "." separated qualified variable names. 
     *  The returned Transition replaces the values of non-observed variables
     *  by their domain.
     **/ 
    static Transition observe (labels_t obs, State potential, const VarOrder & vo) ;
    

    static GShom encapsulate (const HomType & h);

    static long printState (State s, std::ostream & os, long limit, const VarOrder & vo) ;
    
  };

  // user manipulated types
  typedef TPNetSemantics<GHom> dddSemantics;
  typedef TPNetSemantics<GShom> sddSemantics;

  // partial specializations : transitions

  template <>
  inline GHom dddSemantics::getHom (GHom (* foo) (int,int) , int var, int val) {
    return foo(var,val);
  }
  
  template <>
  inline GShom sddSemantics::getHom (GHom (* foo) (int,int) , int var, int val) {
    return localApply( foo(DEFAULT_VAR,val) , var );
  }

  template <>
  inline GShom dddSemantics::encapsulate (const GHom & h) {
    return localApply ( h, DEFAULT_VAR);
  }

  template <>
  inline GShom sddSemantics::encapsulate (const GShom & h) {
    return h;
  }

  // partial specialization : nodes 
  template <>
  inline GSDD sddSemantics::newNode (int var, int val) {
    return GSDD (var, DDD(DEFAULT_VAR, val) );
  }

  template <>
  inline GDDD dddSemantics::newNode (int var, int val) {
    return GDDD (var, val) ;
  }

  template <>
  inline void dddSemantics::getVarIndex (Type::varindex_t & index, int target) {
    index.push_back(DEFAULT_VAR);
    index.push_back(target);
  }

  template <>
  inline void sddSemantics::getVarIndex (Type::varindex_t & index, int target) {
    index.push_back(target);
    index.push_back(DEFAULT_VAR);
  }


  template <>
  inline GSDD dddSemantics::getState (const Marking &m, const VarOrder & vo) {
    return GSDD( DEFAULT_VAR, DDD(getMarking(m,vo)));
  }

  template <>
  inline GSDD sddSemantics::getState (const Marking &m, const VarOrder & vo) {
    return getMarking(m,vo);
  }

  template <>
  inline State sddSemantics::getPotentialStates (State reachable, const VarOrder & vo) {
      // converting to DDD first
      NodeType M0 = NodeType::one;
      // each place = one var as indicated by getPorder
      for (size_t i=0 ; i < vo.size() ; ++i) {
	// retrieve the appropriate place marking
	State dom = extractPotential(i) (reachable);
	// left concatenate to M0
	M0 = dom ^ M0;
      }
      return M0;     
  }

  template <>
  inline State dddSemantics::getPotentialStates (State reachable, const VarOrder & vo) {
      // converting to DDD first
      NodeType M0 = NodeType::one;
      const DDD * reach = (const DDD * ) reachable.begin()->first ;
      // each place = one var as indicated by getPorder
      for (size_t i=0 ; i < vo.size() ; ++i) {
	// retrieve the appropriate place marking
	NodeType dom = computeDomain (i,*reach);
	// left concatenate to M0
	M0 = dom ^ M0;
      }
      return State( DEFAULT_VAR , DDD(M0));         
  }
  
  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  template <>
  inline Transition dddSemantics::observe (labels_t obs, State potential, const VarOrder & vo) {
    if (obs.empty()) {
      return potential;
    }

    its::vars_t obs_index;
    obs_index.reserve(obs.size());
    // each place = one var as indicated by varOrder
    for (int i=vo.size()-1 ; i >= 0  ; --i) {
      Label varname = vo.getLabel(i);
      
      labels_it it = find(obs.begin(), obs.end(),varname);
      if (it != obs.end()) {
	// var is observed
	obs_index.push_back(i);
      } 
    }
    
    return localApply ( observeVars(obs_index,* ( (const DDD *) potential.begin()->first) ), DEFAULT_VAR );
  }

  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  template <>
  inline Transition sddSemantics::observe (labels_t obs, State potential, const VarOrder & vo) {
    if (obs.empty()) {
      return potential ;
    }
    GShom h = GShom::id;

    // each place = one var as indicated by varOrder
    for (int i=vo.size()-1 ; i >= 0  ; --i) {
      Label varname = vo.getLabel(i);
      
      labels_it it = find(obs.begin(), obs.end(),varname);
      if (it == obs.end()) {
	// var is not observed
	
	// Grab potential : one variable long SDD with all values on arc
	SDD pot = extractPotential(i) (potential);
	// Grab arc value and requalify (downcast)  to SDD Dataset type
	SDD potval = * ((const SDD *)  pot.begin()->first); 
	h = h & ( localApply(potval, i) ) ;
      } 
    }
    
    return h;
  }


  template<>
  inline long sddSemantics::printState (State s, std::ostream & os, long limit, const VarOrder & vo) {
    return TypeBasics::printSDDState(s,os,limit,vo);
  }

  template<>
  inline long dddSemantics::printState (State s, std::ostream & os, long limit, const VarOrder & vo) {
    return TypeBasics::printDDDState(s,os,limit,vo);
  }


} // namespace


#endif

