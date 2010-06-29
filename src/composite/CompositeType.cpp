/*
 * CompositeITS.cpp
 *
 *  Created on: 7 déc. 2008
 *      Author: yann
 */

#include "CompositeType.hh"
// exit function
#include <cstdlib>

namespace its {



/** the set InitStates of designated initial states (a copy)*/
labels_t CompositeType::getInitStates () const {
  labels_t ret;
  for (Composite::cstates_it it = comp_.cstates_begin(); it != comp_.cstates_end() ; ++it) {
    ret.push_back(it->first);
  }
  return ret;
}

/** the set T of public transition labels (a copy)*/
labels_t CompositeType::getTransLabels () const {
  std::set<vLabel> toRet;
  for (Composite::syncs_it it = comp_.syncs_begin(); it != comp_.syncs_end() ; ++it) {
      toRet.insert(it->getLabel());
  }
  labels_t ret;
  for (std::set<vLabel>::const_iterator it = toRet.begin() ; it != toRet.end() ; ++it ) {
    ret.push_back(*it);
  }
  return ret;
}

// also sets to default if currently unset.
labels_t CompositeType::getVarSet () const {
  labels_t pnames ;
  for (Composite::comps_it it = comp_.comps_begin() ; it != comp_.comps_end(); ++it ) {
    pnames.push_back(it->getName());
  }
  return pnames;
 }


  Transition CompositeType::getFullShom (const Synchronization & sync) const {
    GShom hsync = GShom::id ;
    const VarOrder * vo = getVarOrder();
    for (Synchronization::parts_it partit = sync.begin() ;
	 partit != sync.end() ;
	 ++partit) {
      Label subname = partit->first;
      Label subtrans = partit->second;
      Shom hpart;
 
      int instindex =  vo->getIndex ( partit->first );
      Composite::comps_it instance = findName( subname, comp_.comps_begin() , comp_.comps_end() );
      labels_t tau;
      tau.push_back(subtrans);
      hpart = localApply(instance->getType()->getSuccs(tau), instindex);
      
      hsync = hpart & hsync;
    }
    
    return hsync;
  }



/** state and transitions representation functions */
/** Local transitions */
Transition CompositeType::getLocals () const {
  std::set<GShom> locals;
  for ( Composite::syncs_it it = comp_.syncs_begin() ; it != comp_.syncs_end(); ++it ) {
    if (it->getLabel() == "") {
      locals.insert(getFullShom(*it));
    }
  }
  /** add subnet locals */
  const VarOrder * vo =  getVarOrder();
  for ( Composite::comps_it it = comp_.comps_begin() ; it != comp_.comps_end() ; ++it ) {
    locals.insert(localApply(it->getType()->getLocals(), vo->getIndex(it->getName())));
  }


  if (! locals.empty())
    return GShom::add(locals);
  else
    return GSDD::null;
}

/** Successors synchronization function : Bag(T) -> SHom.
 * The collection represented by the iterator should be a multiset
 * of transition labels of this type (as obtained through getTransLabels()).
 * Otherwise, an assertion violation will be raised !!
 * */
Transition CompositeType::getSuccs (const labels_t & tau) const {
  GShom h = GShom::id;
  // for each label
  for (labels_it it = tau.begin(); it!= tau.end() ; ++it) {
    // to hold transition (Homs) that match this label
    std::set<GShom> orSet;
    // for each synchronization bearing this label
    for (Composite::syncs_it jt = comp_.syncs_begin() ; jt !=  comp_.syncs_end() ; ++jt) {
      if (jt->getLabel() == *it) {
	// add this sync to sum
	orSet.insert( getFullShom( * jt ) );
      }
    }
    if ( orSet.empty() ) {
      std::cerr << "Unknown transition label " << *it << " when processing getSuccs for type " << comp_.getName() << std::endl;
      assert(false);
    }

    // union the orSet
    Transition toAdd = GShom::add(orSet);
    // compose after current h
    h = toAdd & h ;

  }
  return h;
}

/** To obtain a representation of a labeled state */
State CompositeType::getState(Label stateLabel) const {
  
  Composite::cstates_it it = comp_.cstates_find(stateLabel);
  if (it == comp_.cstates_end()) {
    return State::null;
  }
  GSDD M0 = State::one;
  // the composite state def
  const CState & s = it->second;
  // each place = one var as indicated by getPorder
  for (size_t i=0 ; i < getVarOrder()->size() ; ++i) {
    Label subname = getVarOrder()->getLabel(i);
    // retrieve the appropriate sub instance state
    Label substate = s.getSubState (subname);
    // retrieve the instance specification
    Composite::comps_it instance = findName( subname, comp_.comps_begin() , comp_.comps_end() );
    
    // left concatenate to M0
    M0 = GSDD(i,instance->getType()->getState(substate),M0);
  }
  return M0;
}


Transition CompositeType::getPredicate (Label predicate) const {
  // The predicate should respect the grammar : varName "." .*
  // Where varName is an instance name such as found in getVariableSet(), getVarOrder()
  // "." is the namespace separator and .* represents any sequence of characters. 
  // This function consumes varName"." and passes the rest of the string to the type of the instance designated
    
  // step 1 : parse the predicate
  const char * pred = predicate.c_str();
  vLabel remain, var;
  for (const char * cp = pred ; *cp ; ++cp) {
    if ( *cp == '.' ) {
      remain = cp+1;
      break;
    } else {
      var += *cp;
    }
  }

  int instindex =  getVarOrder()->getIndex ( var );
  if (instindex == -1) {
    std::cerr << "Error variable " + var + " cannot be resolved as an instance name when trying to parse predicate : "  + predicate << std::endl;
    std::cerr << "Failing with error code 2"<< std::endl;
    exit (2);
  }
//   std::cerr << "Composite delegating predicate " << remain << " on instance :"<<var << std::endl;
 
  Composite::comps_it instance = findName( var, comp_.comps_begin() , comp_.comps_end() );
  return localApply( instance->getType()->getPredicate(remain), instindex);
}

  void CompositeType::recPrintState (State s, std::ostream & os, const VarOrder & vo, vLabel str) const {
    if (s == State::one)
      os << "[ " << str << "]"<<std::endl;
    else if(s ==  State::top)
      os << "[ " << str << "T ]"<<std::endl;
    else if(s == State::null)
      os << "[ " << str << "0 ]"<<std::endl;
    else{
      for(State::const_iterator vi=s.begin(); vi!=s.end(); ++vi){
	std::stringstream tmp;
	// Fixme  for pretty print variable names
	Label varname = vo.getLabel(s.variable());
	tmp << varname << "={";
	// the variable should correspond to an instance
	State val =  (const State &) * vi->first;
	const Instance & inst = *comp_.comps_find(varname);
	inst.getType()->printState(val,tmp);
	tmp << "}";
	recPrintState(vi->second, os, vo, str + tmp.str() + " ");
      }
    }
  }

  void CompositeType::printState (State s, std::ostream & os) const {
    vLabel str;
    recPrintState(s,os,*getVarOrder(),str);
  }



} //namespace
