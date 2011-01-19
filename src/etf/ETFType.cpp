
#include "ETFType.hh"
#include <vector>
#include "etf/etf-util.h"

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
    pnames.push_back(vLabel(lts_type_get_state_name(ltstype,i))+":"+lts_type_get_state_type(ltstype,i));
  return pnames;
}


  class ETFTransition {
  public :
    vector<int> proj;
    SDD value;

    ETFTransition (const vector<int> & proj) :  proj(proj),value(SDD::null) {};
    
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

/** state and transitions representation functions */
/** Local transitions */
Transition EtfType::getLocals () const {

  int N=lts_type_get_state_length(ltstype);  

  int nbevents = etf_trans_section_count(etfmodel);


  vector<ETFTransition> transitions;
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


Transition EtfType::getPredicate (Label predicate) const {

  return GShom::id;

//   // The predicate should respect the grammar : varName "." .*
//   // Where varName is an instance name such as found in getVariableSet(), getVarOrder()
//   // "." is the namespace separator and .* represents any sequence of characters. 
//   // This function consumes varName"." and passes the rest of the string to the type of the instance designated
    
//   // step 1 : parse the predicate
//   const char * pred = predicate.c_str();
//   vLabel remain, var;
//   for (const char * cp = pred ; *cp ; ++cp) {
//     if ( *cp == '.' ) {
//       remain = cp+1;
//       break;
//     } else {
//       var += *cp;
//     }
//   }

//   int instindex =  getVarOrder()->getIndex ( var );
//   if (instindex == -1) {
//     std::cerr << "Error variable " + var + " cannot be resolved as an instance name when trying to parse predicate : "  + predicate << std::endl;
//     std::cerr << "Failing with error code 2"<< std::endl;
//     exit (2);
//   }
// //   std::cerr << "Etf delegating predicate " << remain << " on instance :"<<var << std::endl;
 
//   Etf::comps_it instance = findName( var, comp_.comps_begin() , comp_.comps_end() );
//   return localApply( instance->getType()->getPredicate(remain), instindex);

}



  void EtfType::printState (State s, std::ostream & os) const {
    os << "No pretty print yet for states" << std::endl;
  }


}
