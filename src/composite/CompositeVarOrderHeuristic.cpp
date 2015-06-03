#include "composite/CompositeVarOrderHeuristic.hh"

#include "gal/force.hh"
#include "composite/Composite.hh"

namespace its {

/********** lexicographical heuristic *************/

labels_t
lex_heuristic (const Composite & comp)
{
  labels_t pnames ;
  for (Composite::comps_it it = comp.comps_begin() ; it != comp.comps_end(); ++it ) {
    pnames.push_back(it->getName());
  }
  
  // sort attempt to get variables closer together.
  sort(pnames.begin(), pnames.end(), & less_var ) ;
  
  return pnames;
}

/********** force heuristic ****************/

// a type to associate a variable to an int
typedef std::map<std::string,int> cvtoi_t;

class CLocalityEdge : public constraint_t {
  orderHeuristicType strat_;
public:
  CLocalityEdge (const std::set<var_t> & v, orderHeuristicType s)
  : constraint_t (v)
  , strat_ (s)
  {}

  CLocalityEdge (const DDD & v, orderHeuristicType s)
  : constraint_t ()
  , strat_ (s)
  {
    for (DDD::const_iterator it = v.begin() ; it != v.end() ; ++it ) {
      data_.insert(it->first);
    }
  }
  
  float
  cost (const order_t & o) const
  {
    // those strategies are based on a squared cost
    // physical approach, we compute the energy of the springs
    // the energy of a spring is quadratic in its elongation
    if (strat_ == FOLLOW_SQ ||
        strat_ == FOLLOW_DYN_SQ ||
        strat_ == FOLLOW_FDYN_SQ)
    {
      // all the variables are supposed to have the same cog, take the first one
      float c = cog (o).begin ()->second;
      float energy = 0;
      for (const_iterator it = begin ();
           it != end (); ++it)
      {
        float l = ((float)o.find (*it)->second) - c;
        energy += l*l;
      }
      return energy;
    }
    
    // for all other strategies
    // the cost of such a constraint is its span
    // that is the distance between the min index and the max index
    float max = -1, min = o.size ();
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      float i = o.find (*it)->second;
      if (i < min)
      {
        min = i;
      }
      if (i > max)
      {
        max = i;
      }
    }
    return max - min;
  }
  
  cog_t
  cog (const order_t & o) const
  {
    float tmp = 0;
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      tmp += o.find (*it)->second;
    }
    tmp /= (float)data_.size ();
    
    cog_t res;
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      res [*it] = tmp;
    }
    return res;
  }
};
  
  // a set of supporting variables, designated by index
  typedef DDD support_t;
  // a set of supports
  typedef d3::set<DDD>::type support_set_t;
  // a type to hold mappings from labels to sets of supports
  typedef typename hash_map<vLabel, support_set_t>::type labmap_t;

  // a type to hold list of calls
  typedef d3::set<vLabel>::type calls_t;
  // a value type for the result of first pass : for each transition, get an (immediate) support + list of called labels
  typedef std::pair<support_t, calls_t> supp_call_t;
  // a set of these support/call pairs, will be values of our map
  typedef d3::set<supp_call_t>::type supp_call_set_t;
  // a map to hold result of first pass : labels map to sets of pairs (immediate support,calls)
  typedef typename hash_map<vLabel, supp_call_set_t>::type labcallmap_t;
 

static const support_set_t &
computeSupport ( Label label, const labcallmap_t & labcallmap, labmap_t & labmap) {

  // look in labmap first
  labmap_t::accessor res ;
  bool found = labmap.insert(res, label);
  if (! found) {
    // we need to build it from the labcallmap
    
    // access the entry, should exist
    labcallmap_t::const_accessor access;
    labcallmap.find(access, label);
    // iterate over entries
    for (supp_call_set_t::const_iterator it = access->second.begin() ; it != access->second.begin() ; ++it) {
      // it points to a pair (support, list of calls)
      // the result is a set of support,
      support_set_t ressupp;
      // initialize with raw support
      ressupp.insert(it->first);

      // iterate over called labels
      for (calls_t::const_iterator callit = it->second.begin() ; callit != it->second.end() ; ++callit) {

	// recursively resolve supports of callees
	const support_set_t & callsupp = computeSupport(*callit, labcallmap, labmap);
	
	// to build cross-product
	support_set_t cross;
	
	for (support_set_t::const_iterator jt = ressupp.begin() ; jt != ressupp.end() ; ++jt) {
	  for (support_set_t::const_iterator kt = callsupp.begin() ; kt != callsupp.end() ; ++kt) {
	    cross.insert ( *jt + *kt  );
	  }
	}
	// the result after union of callees	
	ressupp = cross;	
      } // end of treating this called label
      res->second.insert(ressupp.begin(), ressupp.end());

    } // for entries in labcallmap

    
  } // ! found


  
  return res->second;


  
}

labels_t
force_heuristic (const Composite & comp, orderHeuristicType strat)
{
  // compute the lexicographical ordering on variables
  // this is used to map variables to integers (for FORCE, variables are int).
  // it will also be used as the initial ordering for FORCE
  labels_t vnames = lex_heuristic (comp);
  
  if (strat == LEXICO)
    return vnames;
  
  cvtoi_t var_to_int;
  for (size_t i = 0; i != vnames.size (); ++i)
  {
    var_to_int [vnames [i]] = i;
  }
  
  std::vector<const constraint_t *> constraints;

  
  // walk the syncs of the Composite
  // in this pass, collect the supports of each transition label by label
  // into this map
  labcallmap_t labcallmap;
  // the set of all called labels, regardless of position
  calls_t called;
  for (Composite::syncs_it it = comp.syncs_begin(); it != comp.syncs_end() ; ++it) {
    supp_call_t suppcall (DDD::null, calls_t() );
    for (Synchronization::parts_it partit = it->begin() ;
	 partit != it->end() ;
	 ++partit) {
	 
      Label subname = partit->first;
     // Label subtrans = partit->second;
      
      if (subname == "self") {
	// Do not follow self calls currently
//	labels_t tau ;
//	tau.push_back(subtrans);
//	hpart = getSuccs(tau);
	suppcall.second.insert(partit->second);
	called.insert(partit->second);
      } else {
	cvtoi_t::const_iterator vi = var_to_int.find ( partit->first );
        if (vi == var_to_int.end ()) {
	  std::cerr << "Could not access index of variable :" <<  partit->first;
	  assert (vi != var_to_int.end ());
	}

	int instindex =  vi->second ;
	//tmp.insert(instindex);
	// insert in the support, DDD style
	suppcall.first = suppcall.first + GDDD(0,instindex);
      }
    }
    
    // build an edge for this transition by itself
    CLocalityEdge * e = new CLocalityEdge (suppcall.first, strat);
    constraints.push_back(e);

    // add this pair to labcallmap values
    typename labcallmap_t::accessor access;
    Label lab = it->getName();
    bool insertion = labcallmap.insert ( access, lab );
    if (insertion) {
      // new key
      access->second = supp_call_set_t () ;
    }
    access->second.insert(suppcall);

  }
  // At the end of this pass, we now have labcallmap :  for each transition, its (immediate) support + list of called labels
  // indexed by labels and the list of callees
  // walk labcallmap to build labmap
  labmap_t labmap;
  for (labcallmap_t::const_iterator access = labcallmap.begin() ; access != labcallmap.end() ; ++access) {
    Label label = access->first;
    // make sure this label is never called locally, hence it is a valid root of the DAG that is the call graph
    if ( called.find(label) != called.end() ) {
      // we need some recursion and a cache to do the traversal effectively
      // this call auto-limits itself to avoid explosion of number of constraints
      // it read-only accesses labcallmap to build up labmap, also used as a cache
      const support_set_t & supports = computeSupport( label, labcallmap, labmap);

      // iterate over the result and build constraints
      for (support_set_t::const_iterator suppit = supports.begin() ; suppit != supports.end() ; ++suppit ) {
	CLocalityEdge * e = new CLocalityEdge (*suppit, strat);
	constraints.push_back(e);
	
      }

    }
  }
 
    
  // build the initial ordering from the lexicographical ordering
  order_t init_order;
  int i = 0;
  for (labels_t::const_iterator it = vnames.begin ();
       it != vnames.end (); ++it)
  {
    init_order [var_to_int [*it]] = i++;
  }
  
  
  // \debug
  std::map<int, std::string> int_to_var;
  for (std::map<std::string,int>::const_iterator it = var_to_int.begin ();
       it != var_to_int.end (); ++it)
  {
    int_to_var[it->second] = it->first;
  }
  
  // call the FORCE algorithm
  order_t new_order = force (constraints, init_order);
  
  // rebuild a labels_t according to the order given by FORCE
  labels_t result (new_order.size ());
  for (order_t::const_iterator it = new_order.begin ();
       it != new_order.end (); ++it)
  {
    result [it->second] = vnames [it->first];
  }
  
  // delete the constraints
  for (std::vector<const constraint_t *>::const_iterator it = constraints.begin ();
       it != constraints.end (); ++it)
  {
    delete *it;
  }
  
  return result;
}

} // namespace its
