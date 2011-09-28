
#include "ETFType.hh"
#include <vector>
extern "C" {
#include "etf/etf-util.h"
#include "etf/chunk_support.h"
}
#include "Hom_Basic.hh"

#include <algorithm>

using std::vector;

namespace its {



  EtfType::EtfType (Label path) : name(path) {
    etfmodel = etf_parse_file(path.c_str());
    ltstype =  etf_type(etfmodel);
  }


/** the set InitStates of designated initial states (a copy)*/
labels_t EtfType::getInitStates () const {
  labels_t ret (1,"init");
  return ret;
}

/** the set T of public transition labels (a copy)*/
labels_t EtfType::getTransLabels () const {
  return labels_t();
}

// also sets to default if currently unset.
labels_t EtfType::getVarSet () const {
  int N=lts_type_get_state_length(ltstype);  
  labels_t pnames ;
  pnames.reserve(N);
  for (int i=0; i < N ; i++)
    pnames.push_back(vLabel(lts_type_get_state_name(ltstype,i))  ); // +":"+lts_type_get_state_type(ltstype,i));
  return pnames;
}


  class ETFTransition {
  public :
    vector<int> proj;
    SDD value;

    ETFTransition (const vector<int> & proj) :  proj(proj),value(SDD::null) {};
    
    // returns the index of the variable in the projection
    // e.g. if transition touches vars 0, 3 , 5 and variable 3 is the target pid "var"
    // this will return 1 (i.e index of 3 in projection)
    // return -1 if var is not touched by the transition
    int concernsVariable (int var) const {
      for (vector<int>::const_iterator it = proj.begin() ; it != proj.end() ; ++it ) {
	if (*it == var) {
	  return var;
	}
      }
      return -1;
    }

    void addEdge (int *src, int * dst) {
      GSDD element = SDD::one;
      int len = proj.size();

      for (int i = 0; i < len; i++) {
	element = SDD(proj[i], DDD(DEFAULT_VAR,src[i],GDDD(DEFAULT_VAR,dst[i])) , element);
      }

      value = value + element;
      assert(value != SDD::top);
    }

    GShom getShom () const {
      return apply2k(value);
    }

    
  };


  void EtfType::compute_transitions (vector<ETFTransition> & transitions) const {
    int N=lts_type_get_state_length(ltstype);  
    
    int nbevents = etf_trans_section_count(etfmodel);
    // Initialize the relation
    
    for(int section=0;section<nbevents;section++){
      
      //    printf("group %4d/%d\n",section+1,nbevents);
      
      etf_rel_t trans=etf_trans_section(etfmodel,section);
      
      int src[N];
      int dst[N];
      int lbl[nbevents];
      
      vector<int> proj;
      
      ETFrelIterate(trans);
      if (ETFrelNext(trans,src,dst,lbl)) {
	for(int i=0 ; i < N; i++) {
	  if (src[i]) {
	    proj.push_back(i);
	  }
	}
      }
      int len = proj.size();
      
      transitions.push_back( ETFTransition(proj) );
      
      
      ETFrelIterate(trans);
      while(ETFrelNext(trans,src,dst,lbl)){
	int psrc[len];
	int pdst[len];
	
	for (int ii=0 ; ii < len ; ++ii) {
	  psrc[ii] = src[proj[ii]]-1;
	  pdst[ii] = dst[proj[ii]]-1;
	}
	
	transitions[section].addEdge(psrc,pdst);
      }
    }
    
  }
  
/** state and transitions representation functions */
/** Local transitions */
Transition EtfType::getLocals () const {


  vector<ETFTransition> transitions;
  compute_transitions(transitions);

  d3::set<GShom>::type toadd;
  for (vector<ETFTransition>::const_iterator it = transitions.begin() ; it != transitions.end() ; ++it ){
    toadd.insert(it->getShom());
  }

  return GShom::add(toadd);
}

/** Successors synchronization function : Bag(T) -> SHom.
 * The collection represented by the iterator should be a multiset
 * of transition labels of this type (as obtained through getTransLabels()).
 * Otherwise, an assertion violation will be raised !!
 * */
Transition EtfType::getSuccs (const labels_t & tau) const {
  return GShom::id;
}

/** To obtain a representation of a labeled state */
State EtfType::getState(Label stateLabel) const {
  int N=lts_type_get_state_length(ltstype);  
  int src[N];
  etf_get_initial(etfmodel,src);

  GSDD element = SDD::one;
  for (int i = 0 ; i < N ; i++)
    element = GSDD(i, DDD(DEFAULT_VAR,src[i]), element);
  
  return element;
}

State EtfType::getPotentialStates(State reachable) const {
  int N=lts_type_get_state_length(ltstype);  

  GSDD element = SDD::one;
  for (int i = 0 ; i < N ; i++)
    element = (extractPotential(i) (reachable))  ^ element;
  
  return element;
  
}


  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables, 
   *  in standard "." separated qualified variable names. 
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/ 
  Transition EtfType::observe (labels_t obs, State potential) const {
    if (obs.empty()) {
      return potential;
    }
    
    GShom h = GShom::id;
    const VarOrder & vo = * getVarOrder();
    // each place = one var as indicated by varOrder
    for (int i=vo.size()-1 ; i >= 0  ; --i) {
      Label varname = vo.getLabel(i);
      
      labels_it it = find(obs.begin(), obs.end(),varname);
      if (it == obs.end()) {
	// var is not observed
	
	// Grab potential : one variable long SDD with all values on arc
	SDD pot = extractPotential(i) (potential);

//	std::cerr << "i " << i << "  pot : " << pot << std::endl;

	// Grab arc value and requalify (downcast)  to SDD Dataset type
	DDD potval = * ((const DDD *)  pot.begin()->first); 
	h = h & ( localApply(potval , i) ) ;
      } 
    }
    
    return h;
  }


 /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void EtfType::getNamedLocals (namedTrs_t & ntrans) const {
    vector<int> proc_vars;
    labels_t proc_names;

    int N=lts_type_get_state_length(ltstype);  

    for (int i=0; i < N ; i++) {
      vLabel var2 = lts_type_get_state_name(ltstype,i);
      vLabel type = lts_type_get_state_type(ltstype,i);
            
      if (type != "byte") {
	proc_vars.push_back(i);
	proc_names.push_back(var2);
//	std::cerr << "we have a process of type :" << var2 << ":" << type << std::endl ;
      }
    }
    
    vector<ETFTransition> transitions;
    compute_transitions(transitions);

    d3::set<GShom>::type toadd;

    
    // To hold a map from transition name "Env.obs->move" to transition homomorphism
    typedef std::map<vLabel,Transition> namedTs_t;
    typedef namedTs_t::const_iterator namedTs_it;
    namedTs_t namedTs;


    for (vector<ETFTransition>::const_iterator it = transitions.begin() ; it != transitions.end() ; ++it ){
      // find "owner" process
      int process = -1;
      // index of owner process in transition projection
      int pproj = -1;
      for (vector<int>::const_iterator pid = proc_vars.begin() ; pid != proc_vars.end() ; ++pid) {
	if ( (pproj = it->concernsVariable(*pid)) != -1) {
	  process = *pid;
	  break;
	}
      }
      
      // Every transition should belong to a process
      assert( process != -1 );

      // find state pair 
      vLabel srcS;
      vLabel targetS;

      // source process state index
      int src = -1;
      // target process state index
      int dest = -1;

      // Grab the SDD that represents the 2k mapping for the process state variable
      State procTr = extractPotential ( pproj ) ( it->value ) ;

//      std::cerr << "wroking on 2k SDD : " << procTr << std::endl;
      
      // downcast to DDD
      const DDD * val = (const DDD *) (procTr.begin()->first) ;

      // read values
      src = val->begin()->first ;

      dest = val->begin()->second.begin()->first ;
      
      // build a name for this transition, of the form : "P_Rbt1.observe->move"
      vLabel tname = lts_type_get_state_name(ltstype,process) ;
      tname += ".";
      
//      std::cerr << "building a transition for process : " << tname << "  " << src << "/" << dest << std::endl;


      // find the type id for this process
      int typeId = -1;

      int type_count=lts_type_get_type_count(ltstype);
      vLabel procType = lts_type_get_state_type(ltstype,process);
      for(int i=0;i<type_count;i++){
	//      Warning(info,"dumping type %s",lts_type_get_type(ltstype,i));
	vLabel type2 = lts_type_get_type(ltstype,i);
	//      std::cerr << "testing type " << type2 <<" against "<< type << std::endl;
	if (type2 == procType) {
	  typeId = i;
	  break ;
	}
      }

      
      {
	chunk c=etf_get_value(etfmodel,typeId,src);
	size_t len=c.len*2+3;
	char str[len];
	chunk2string(c,len,str);
	
	tname += str; 
      }
      tname += "->";
      {
	chunk c=etf_get_value(etfmodel,typeId,dest);
	size_t len=c.len*2+3;
	char str[len];
	chunk2string(c,len,str);

	tname += str ; 
      }

      // add to other transitions with same label
      Transition effect = namedTs[tname] ;
      if (effect == Transition::id) {
	effect = Transition::null;
      }
      effect = effect + it->getShom();
      namedTs[tname] = effect ;
    }

    // return  the pairs that consitute the map
    for (namedTs_it it = namedTs.begin() ; it != namedTs.end() ; ++it ) {
      ntrans.push_back(namedTr_t(it->first , it->second ));
    }
    
    // Old version, very simple
    // ntrans.push_back(namedTr_t("ETF edges",getLocals()));
  }

Transition EtfType::getAPredicate (Label predicate) const {
  // The predicate should respect the grammar : varName "=" .*
  // NB : NO whitespace allowed anywhere in the predicate string, or they will be parsed as part of identifiers.
  // Where varName is an instance name such as found in getVariableSet(), getVarOrder()
  // If the type of the variable is "byte" we expect to see an integer on the right.
  // If the type of the variable is some ad hoc type we expect to see the name of a value of that type on the right.

  // step 1 : parse the predicate
  const char * pred = predicate.c_str();
  vLabel remain, var;
  for (const char * cp = pred ; *cp ; ++cp) {
    if ( *cp == '=' ) {
      remain = cp+1;
      break;
    } else {
      var += *cp;
    }
  }
  
  int N=lts_type_get_state_length(ltstype);  
  int varindex = -1;
  for (int i=0; i < N ; i++) {
    vLabel var2 = lts_type_get_state_name(ltstype,i);
    if (var2 == var) {
      varindex = i;
      break;
    }
  }
  if (varindex == -1) {
    std::cerr << "Error variable " + var + " cannot be resolved as an instance name when trying to parse predicate : "  + predicate << std::endl;
    std::cerr << "Expected one of : " ;
    for (int i=0; i < N ; i++) {
      std::cerr << lts_type_get_state_name(ltstype,i) << ", ";
    }
    std::cerr << "\nFailing with error code 2"<< std::endl;
    exit (2);
  }
  // This is the actual index given the current Ordering
  int instindex =  getVarOrder()->getIndex ( var );
  
  // Find the type 
  vLabel type = lts_type_get_state_type(ltstype,varindex);
  if ( type == "byte" || type == "int") {
    int value = 0 ;
    if ( sscanf (remain.c_str(), "%d" , &value) == 0 ) {
      std::cerr << "Unable to parse right hand side of comparison as an integer : " << remain << " when parsing predicate " << predicate << std::endl;
      std::cerr << "Error is fatal, failing with error code 2" << std::endl;
      exit (2);      
    }
//    std::cerr << "Ok for predicate : var"<<instindex<< " = " << value << std::endl;
    return localApply(varEqState (DEFAULT_VAR, value), instindex);
  } else {
    int type_count=lts_type_get_type_count(ltstype);
    for(int i=0;i<type_count;i++){
//      Warning(info,"dumping type %s",lts_type_get_type(ltstype,i));
      vLabel type2 = lts_type_get_type(ltstype,i);
//      std::cerr << "testing type " << type2 <<" against "<< type << std::endl;
      if (type2 == type) {
	// Ids are quoted in LTSmin
	remain = "\"" + remain + "\"";
	// Found the right type
	int values=etf_get_value_count(etfmodel,i);
	for(int j=0;j<values;j++){
	  chunk c=etf_get_value(etfmodel,i,j);
	  size_t len=c.len*2+3;
	  char str[len];
	  chunk2string(c,len,str);
//	  std::cerr << "testing value " << str <<" against "<< remain << std::endl;
	  
	  if (vLabel(str) == remain ) {
	    // Hit !
//	    std::cerr << "Ok for predicate : var"<<instindex<< " = " << j<< std::endl;
	    return localApply(varEqState (DEFAULT_VAR, j), instindex);
	  }	  
	}
	{
	  std::cerr << "Unable to find value \""<< remain << "\" in type "<< type << " of variable " << var ;
	  std::cerr << " when parsing predicate " << predicate << std::endl;
	  std::cerr << "Expected a value among :" ;
	  int values=etf_get_value_count(etfmodel,i);
	  for(int j=0;j<values;j++){
	    chunk c=etf_get_value(etfmodel,i,j);
	    size_t len=c.len*2+3;
	    char str[len];
	    chunk2string(c,len,str);
	    std::cerr << str << "  ;";
	  }
	  
	  std::cerr << std::endl << "Error is fatal, failing with error code 2" << std::endl;
	  exit (2);      
	}	
      }
    }
      {
	std::cerr << "Unable to find type \""<< type << "\" in type declarations of model " ;
	std::cerr << " when parsing predicate " << predicate << std::endl;
	std::cerr << "Error is fatal, failing with error code 2" << std::endl;
	exit (2);      
      }	
    
  }
  // to please the compiler
  return GShom::id;
}  


  void EtfType::printState (State s, std::ostream & os) const {
    os << "No pretty print yet for states" << std::endl;
  }


}
