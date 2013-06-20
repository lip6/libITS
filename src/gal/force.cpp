#include "force.hh"

#include <vector>
#include <algorithm>

#include <assert.h>

#include <iostream>

// a pos_t associates to each variable a position
typedef std::map<var_t,float> pos_t;

// utility function to sort according to the second component of the pairs
bool
second_sort (const std::pair<var_t, float> & a, const std::pair<var_t, float> &b)
{
  return a.second < b.second;
}

// utility function to rebuild an order_t from the new locations
// sorts according to new locations (float) and converts to order_t (int)
order_t
order_from_newloc (const pos_t & new_loc)
{
  std::vector< std::pair< var_t, float > > tmp (new_loc.begin (), new_loc.end ());
  std::sort (tmp.begin (), tmp.end (), second_sort);
  
  // rebuild an order_t
  order_t res;
  int i = 0;
  for (std::vector< std::pair< var_t, float > >::const_iterator it = tmp.begin ();
       it != tmp.end (); ++it)
  {
    res[it->first] = i++;
  }
  return res;
}

// utility function to compute the total cost of a set of constraints
int
cost (const std::vector<const constraint_t *> & v,
      const order_t & o)
{
  int res = 0;
  for (std::vector<const constraint_t *>::const_iterator it = v.begin ();
       it != v.end (); ++it)
  {
    res += (*it)->cost (o);
  }
  return res;
}

// associates to each variable the number of adjacent constraints
typedef std::map<var_t,int> nb_edge_t;

// one step of iteration
// takes a set of constraint, an ordering, and the number of constraint for each
// variable (to avoid to recompute it each time)
order_t
new_order (const std::vector<const constraint_t *> & c,
           const order_t & o,
           const nb_edge_t & n)
{
  pos_t new_loc;
  // for each constraint
  for (std::vector<const constraint_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    // compute its center of gravity
    float cog = (*it)->cog (o);
    // add it to each of adjacent variable
    for (constraint_t::const_iterator ei = (*it)->begin ();
         ei != (*it)->end (); ++ei)
    {
      new_loc [*ei] += cog;
    }
  }
  // if a variable has not been visited, set its new location to its current one
  for (order_t::const_iterator it = o.begin ();
       it != o.end (); ++it)
  {
    if (new_loc [it->first] == 0)
    {
      new_loc [it->first] = it->second;
    }
  }
  
  assert (new_loc.size () == o.size ());
  // divide by the number of adjacent constraints to make the average
  for (nb_edge_t::const_iterator it = n.begin ();
       it != n.end (); ++it)
  {
    if (it->second != 0)
      new_loc [it->first] /= it->second;
  }
  
  // result
  return order_from_newloc (new_loc);
}

// a variant of a step of iteration, where the constraints whose cost is null
// are not taken into account
// takes a set of constraints and an ordering
order_t
new_order_neutral (const std::vector<const constraint_t *> & c,
                   const order_t & o)
{
  pos_t new_loc;
  // the number of edges adjacent to each variable must be recomputed
  nb_edge_t n;
  for (std::vector<const constraint_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    if ((*it)->cost (o) != 0)
    {
      float cog = (*it)->cog (o);
      for (constraint_t::const_iterator ei = (*it)->begin ();
           ei != (*it)->end (); ++ei)
      {
        new_loc [*ei] += cog;
        n [*ei]++;
      }
    }
  }
  // if a variable has not been visited, set its new location to its current one
  for (order_t::const_iterator it = o.begin ();
       it != o.end (); ++it)
  {
    if (new_loc [it->first] == 0)
      new_loc [it->first] = it->second;
  }
  
  assert (new_loc.size () == o.size ());
  // divide by the number of adjacent constraints to make the average
  for (nb_edge_t::const_iterator it = n.begin ();
       it != n.end (); ++it)
  {
    if (it->second != 0)
      new_loc [it->first] /= it->second;
  }
  
  // result
  return order_from_newloc (new_loc);
}

order_t
force (const std::vector<const constraint_t *> & c,
       const order_t & io)
{
  // \debug
  std::cerr << "initial cost " << cost (c, io) << std::endl;
  
  // compute the number of edges for each variable
  nb_edge_t n;
  for (std::vector<const constraint_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    for (constraint_t::const_iterator ei = (*it)->begin ();
         ei != (*it)->end (); ++ei)
    {
      n[*ei]++;
    }
  }
  
  order_t res = io;
  order_t no;
  // arbitrarily fixed maximal number of iterations
  size_t limit = io.size ();
  while (limit-- != 0)
  {
    // make a step
    no = new_order (c, res, n);
    // if the cost is not decreased
    if (cost (c, no) >= cost (c, res))
    {
      // try a step without taking into account constraint with a null cost
      no = new_order_neutral (c, res);
      // if the cost is not decreased
      if (cost (c, no) >= cost (c, res))
      {
        // leave the loop, we have a local minimum
        break;
      }
    }
    res = no;
  }
  
  // \debug
  std::cerr << "final cost " << cost (c, res) << std::endl;
  
  return res;
}
