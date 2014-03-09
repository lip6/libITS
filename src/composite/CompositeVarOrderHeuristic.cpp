#include "CompositeVarOrderHeuristic.hh"

#include "gal/force.hh"
#include "composite/Composite.hh"

namespace its {

/********** lexicographical heuristic *************/

static int parseFirstInt (Label s) {
  int val = -1 ;
  const char * sc = s.c_str();
  for (const char  *cp = sc ; *cp ; ++ cp ) {
    if (*cp >= '0' && *cp <= '9'){
      sscanf (cp, "%d" , &val);
    }
  }
  return val;
}


labels_t
lex_heuristic (const Composite * comp const g)
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
force_heuristic (const Composite * const comp, orderHeuristicType strat)
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
	for (Synchronization::parts_it partit = sync.begin() ;
	 partit != sync.end() ;
	 ++partit) {
	 
      Label subname = partit->first;
      Label subtrans = partit->second;
      
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
      
	  
	  toRet.insert(it->getLabel());
  }
  
  for (GAL::trans_it it = g->trans_begin ();
       it != g->trans_end (); ++it)
  {
    // add locality constraint
    add_locality_constraint (constraints, g, strat, *it, var_to_int);
    // add query constraint
    add_query_constraint (constraints, g, *it, var_to_int);
  }
  
  // build the initial ordering from the lexicographical ordering
  order_t init_order;
  int i = 0;
  for (labels_t::const_iterator it = vnames.begin ();
       it != vnames.end (); ++it)
  {
    init_order [var_to_int [*it]] = i++;
  }
  
  if (strat == SATUR)
  {
    // count the number of constraints that have the same set of variables
    std::map<std::set<var_t>, int> count;
    for (std::vector<const constraint_t *>::const_iterator it = constraints.begin ();
         it != constraints.end (); ++it)
    {
      count [(*it)->get_data ()] += 1;
    }
    for (std::vector<const constraint_t *>::const_iterator it = constraints.begin ();
         it != constraints.end (); ++it)
    {
//      std::cerr << count[(*it)->get_data ()] << std::endl;
      const_cast<constraint_t *> (*it)->set_dev (count[(*it)->get_data ()]);
    }
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
