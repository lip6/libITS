/*
 * CompositeITS.cpp
 *
 *  Created on: 7 déc. 2008
 *      Author: yann
 */

#include "its/composite/CompositeType.hh"
#include "its/composite/CompositeVarOrderHeuristic.hh"
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

  pnames = force_heuristic(comp_, DEFAULT);

  // // DEBUG
  //   for (labels_t::const_iterator it = pnames.begin ();
  //        it != pnames.end (); ++it)
  //   {
  //     std::cerr << (*it) << ",";
  //   }
  //   std::cerr << std::endl;
  

  return pnames;
 }

Transition CompositeType::skipLocalApply(Transition h, int index) const {
	if (comp_.comps_size()==1) {
		assert(index == 0);
		return h;
	} else {
		return localApply(h,index);
	}
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
      
      if (subname == "self") {
	labels_t tau ;
	tau.push_back(subtrans);
	hpart = getSuccs(tau);
      } else {
	int instindex =  vo->getIndex ( partit->first );
	Composite::comps_it instance = findName( subname, comp_.comps_begin() , comp_.comps_end() );
	labels_t tau;
	tau.push_back(subtrans);
	hpart = skipLocalApply(instance->getType()->getSuccs(tau), instindex);
      }
      
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
    locals.insert(skipLocalApply(it->getType()->getLocals(), vo->getIndex(it->getName())));
  }


  if (! locals.empty())
    return GShom::add(locals);
  else
    return GSDD::null;
}


  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void CompositeType::getNamedLocals (namedTrs_t & locals) const {
    for ( Composite::syncs_it it = comp_.syncs_begin() ; it != comp_.syncs_end(); ++it ) {
      if (it->getLabel() == "") {
	locals.push_back(namedTr_t(it->getName(),getFullShom(*it)));
      }
    }
    /** add subnet locals */
    // if detailed, recurse in nested types, else just make one event labeled "iname.locals"
    bool detailed = true;
    const VarOrder * vo =  getVarOrder();    
    
    if (!detailed) {
      for ( Composite::comps_it it = comp_.comps_begin() ; it != comp_.comps_end() ; ++it ) {
	locals.push_back(namedTr_t(it->getName()+".locals",  skipLocalApply(it->getType()->getLocals(), vo->getIndex(it->getName()))));
      }
    } else { 
      // detailed

      for ( Composite::comps_it it = comp_.comps_begin() ; it != comp_.comps_end() ; ++it ) {
	namedTrs_t loc_t;
	it->getType()->getNamedLocals(loc_t);      
	int varindex =  vo->getIndex(it->getName());
	vLabel iname = it->getName()+".";
	for (namedTrs_it jt=loc_t.begin() ; jt != loc_t.end() ; ++jt) {
	  locals.push_back(namedTr_t(iname + jt->first , skipLocalApply(jt->second, varindex)));
	}
      }      
    }
      
    
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
      return Transition::null;
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

    if (comp_.comps_size()==1) {
    	return instance->getType()->getState(substate);
    }
    // left concatenate to M0
    M0 = GSDD(i,instance->getType()->getState(substate),M0);
  }
  return M0;
}


  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State CompositeType::getPotentialStates(State reachable) const {
    if (comp_.comps_size() == 1) {
  		  Label subname = getVarOrder()->getLabel(0);
		  // retrieve the instance specification
		  Composite::comps_it instance = findName( subname, comp_.comps_begin() , comp_.comps_end() );
		  return instance->getType()->getPotentialStates(reachable);
    } else {
	  GSDD M0 = State::one;
	  // each place = one var as indicated by getPorder
	  for (size_t i=0 ; i < getVarOrder()->size() ; ++i) {
		  State pot = extractPotential(i) (reachable);
		  const DataSet * vals = pot.begin()->first;

		  Label subname = getVarOrder()->getLabel(i);
		  // retrieve the instance specification
		  Composite::comps_it instance = findName( subname, comp_.comps_begin() , comp_.comps_end() );

		  State edgepot = instance->getType()->getPotentialStates( * ((const SDD *) vals) );
		  // left concatenate to M0
		  M0 = GSDD(i,edgepot)  ^ M0;
	  }
	  return M0;
    }
  }


  
  // Returns a pair "instance name" "remains of string" by breaking on "." or resolving as nested variables.
  CompositeType::splitvar_t CompositeType::splitVar (vLabel predicate) const {
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
      } else if (*cp == '=' || *cp == '<' || *cp == '>' || *cp== '!') {
	remain = cp;
	break;
      } else {
	var += *cp;
      }
    }
    int instindex =  getVarOrder()->getIndex ( var );
    if (instindex == -1) {

      // Try to recuperate by interpreting the variable as a nested exposed variable
      vLabel subcomp = comp_.exposedIn (var);
      if ( subcomp != "") {
	// restructure input data
	remain = var + remain;
	// the subcomp is the var now
	var = subcomp;
      } else {
	
	std::cerr << "Error variable " << var << " cannot be resolved as an instance name or as an exposed sub variable when trying to parse predicate :'"  <<  predicate << "'" << std::endl;
	std::cerr << "Failing with error code 2"<< std::endl;
	exit (2);
      }
    }
    
    return std::make_pair(var, remain);
  }


  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition CompositeType::observe (labels_t obs, State potential) const {

    // Map components to observed variables within them
    typedef std::map<int, labels_t> instvars_t;

    instvars_t instvars;
    
    for (labels_it it = obs.begin() ; it != obs.end() ; ++it ) {
      
      splitvar_t vv = splitVar(*it);  
      Label var = vv.first;
      Label remain = vv.second;
      int instindex =  getVarOrder()->getIndex (var);      

      // insert in map
      labels_t & tab = instvars[instindex];
      tab.push_back(remain);
      
    }

    Transition h = Transition::id;
     // each place = one var as indicated by varOrder
    const VarOrder & vo = *getVarOrder();
    for (size_t i=vo.size()-1 ; i >= 0  ; --i) {
      labels_t & tab = instvars[i];
      // there are some observed vars inside
      Composite::comps_it instance = findName( vo.getLabel(i), comp_.comps_begin() , comp_.comps_end() );

      if (comp_.comps_size() == 1) {
    	  h = h & instance->getType()->observe(tab,potential);
      } else {
    	  // Grab potential : one variable long SDD with all values on arc
      
    	  State pot = extractPotential(i) (potential);
      
    	  // Grab arc value and requalify (downcast)  to SDD Dataset type
    	  State potval = * ((const State *)  pot.begin()->first);
    	  h = h & ( localApply( instance->getType()->observe(tab,potval) , i) ) ;
      }
    }  
    
    return h;    
  }

  /** Return the index of a given variable in the representation, actually a vector of indices in SDD case.
   */
  void CompositeType::getVarIndex(varindex_t & index, Label vname) const {
    std::istringstream iss(vname);
    vLabel var;
    std::getline(iss, var, '.');
    vLabel endOf;
    std::getline(iss, endOf);

    int instindex =  getVarOrder()->getIndex (var);
    if (comp_.comps_size()!=1) {
      index.push_back(instindex);
    }
    // std::cout << "Var " << var << " Instance index :" << instindex;
    // std::cout << "end of var :" << endOf << std::endl;
    Composite::comps_it instance = findName( var, comp_.comps_begin() , comp_.comps_end() );
    instance->getType()->getVarIndex(index, endOf);
  }



Transition CompositeType::getAPredicate (Label predicate) const {
    
//   std::cerr << "Composite delegating predicate " << remain << " on instance :"<<var << std::endl;
  // the varindex 
  //  std::cerr << "pred:"<<predicate<<":\n"<<std::flush;
  if (predicate == "true") {
    return Transition::id;
  } else if (predicate == "false") {
    return Transition::null;
  }

  splitvar_t vv = splitVar(predicate);  
  Label var = vv.first;
  Label remain = vv.second;
  int instindex =  getVarOrder()->getIndex (var);

  Composite::comps_it instance = findName( var, comp_.comps_begin() , comp_.comps_end() );
  char  buff [remain.size()+ 1];
  strcpy(buff,remain.c_str());
  return skipLocalApply( instance->getType()->getPredicate(buff), instindex);
}

  void CompositeType::recPrintState (State s, std::ostream & os, const VarOrder & vo, vLabel str) const {
    if (s == State::one)
      os << "[ " << str << "]"<<std::endl;
    else if(s ==  State::top)
      os << "[ " << str << "T ]"<<std::endl;
    else if(s == State::null)
      os << "[ " << str << "0 ]"<<std::endl;
    else{
    	if (comp_.comps_size()==1) {
    		// Fixme  for pretty print variable names
    		Label varname = vo.getLabel(0);
    		os << varname << "={";
    		const Instance & inst = *comp_.comps_find(varname);
    		inst.getType()->printState(s,os);
    		os << "}";
    	} else {
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
  }

  void CompositeType::printState (State s, std::ostream & os) const {
    vLabel str;
    recPrintState(s,os,*getVarOrder(),str);
  }



} //namespace
