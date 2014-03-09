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
  for (Composite::syncs_it it = comp.syncs_begin(); it != comp.syncs_end() ; ++it) {
    std::set<var_t> tmp;
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
	
      } else {
	cvtoi_t::const_iterator vi = var_to_int.find ( partit->first );
        if (vi == var_to_int.end ()) {
	  std::cerr << "Could not access index of variable :" <<  partit->first;
	  assert (vi != var_to_int.end ());
	}

	int instindex =  vi->second ;
	tmp.insert(instindex);
      }
    }
    CLocalityEdge * e = new CLocalityEdge (tmp, strat);
    constraints.push_back(e);
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
