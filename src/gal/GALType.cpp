#include "GALType.hh"

#include "Observe_Hom.hh"
#include "ExprHom.hpp"

#include <algorithm>

#define DEFAULT_VAR 0

namespace its {

/** the set T of public transition labels (a copy)*/
labels_t GALType::getTransLabels () const {
  labels_t ret;

  for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
    ret.push_back(it->getName());
  }
  return ret;
}

  /** To obtain a representation of a labeled state */
  State GALType::getState(Label stateLabel) const {
    // converting to DDD first
    DDD M0 = DDD::one;
    const VarOrder & vo = *getVarOrder();
    // each place = one var as indicated by getPorder
    for (size_t i=0 ; i < vo.size() ; ++i) {
      Label pname = vo.getLabel(i);
      // retrieve the appropriate place marking
      int mark = gal_->getVarValue(pname);
      // left concatenate to M0
      M0 = DDD (i,mark) ^ M0;
      // for pretty print
      DDD::varName(i,pname);
    }
    return State(DEFAULT_VAR, M0);
  }

  GHom GALType::buildHom(const GuardedAction & it) const {
        
    GHom guard = predicate ( it.getGuard(), getVarOrder());
    GHom action = GHom::id;
    for (GuardedAction::actions_it jt = it.begin() ; jt != it.end() ; ++ jt) {
      action = assignExpr(jt->getVariable(), jt->getExpression(),getVarOrder()) & action;
    }
    return action & guard;
  }

    /** state and transitions representation functions */
  /** Local transitions */
  Transition GALType::getLocals () const {

    d3::set<GHom>::type toadd;

    for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
      toadd.insert(buildHom(*it) );
    }
    GHom sum = GHom::add(toadd);

    // so "sum" contains the successor relationship
    // now factor in the transient states
    GHom next = fixpoint(ITE ( predicate(gal_->isTransientState(), getVarOrder()), sum, GHom::id)) & sum;

    return localApply(  next, DEFAULT_VAR );
  }

  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void GALType::getNamedLocals (namedTrs_t & ntrans) const {
    for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
      ntrans.push_back(std::make_pair(it->getName() , localApply(buildHom(*it),DEFAULT_VAR)) );
    }
  }

  

  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State GALType::getPotentialStates(State reachable) const {
    // Note : code copy pasted from TPNetSemantics, refactor to share ?

    const VarOrder & vo = * getVarOrder();
    // converting to DDD first
    DDD M0 = DDD::one;
    const DDD * reach = (const DDD * ) reachable.begin()->first ;
    // each place = one var as indicated by getPorder
    for (size_t i=0 ; i < vo.size() ; ++i) {
      // retrieve the appropriate place marking
      DDD dom = computeDomain (i,*reach);
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
  Transition GALType::observe (labels_t obs, State potential) const {
    if (obs.empty()) {
      return potential;
    }

    const VarOrder & vo = *getVarOrder();

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


  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition GALType::getAPredicate (Label predicate) const {
  
    
    AtomicPredicate pred = parseAtomicPredicate(predicate);
    
    
    //      std::cerr << "Petri net parsed predicate var:" << var << " comp:" << comp << " value:"<<val <<std::endl;
    //      std::cerr << "Translates to hom :" << Semantics::getHom ( foo, index, value) << std::endl;
    
    return  localApply( (*pred.comp) (pred.var , pred.val), DEFAULT_VAR );
  }


} // namespace
