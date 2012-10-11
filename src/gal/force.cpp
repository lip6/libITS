#include "force.hh"

#include <vector>
#include <algorithm>

#include <assert.h>

#include <iostream>

typedef std::map< std::string, int > nb_edge_t;

// utility function for sorting according to the second component of the pairs
bool
second_sort (const std::pair<std::string,float> & a, const std::pair<std::string,float> & b)
{
  return a.second < b.second;
}

// utility function to rebuild an order_t from the new locations
// sorts according to the new locations and converts to order_t
order_t
order_from_newloc (const std::map< std::string, float > & new_loc)
{
  std::vector< std::pair< std::string, float > > tmp (new_loc.begin (), new_loc.end ());
  std::sort (tmp.begin (), tmp.end (), second_sort);
  
  // rebuild an order
  order_t res;
  int i = 0;
  for (std::vector< std::pair< std::string, float > >::const_iterator it = tmp.begin ();
       it != tmp.end (); ++it)
  {
    res[it->first] = i++;
  }
  
  return res;
}

order_t
new_order (const std::vector<const edge_t *> & c,
           const order_t & o,
           const nb_edge_t & n)
{
  std::map< std::string, float > new_loc;
  for (std::vector<const edge_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    cog_t cog = (*it)->cog (o);
    for (edge_t::const_iterator ei = (*it)->begin ();
         ei != (*it)->end (); ++ei)
    {
      new_loc[*ei] += cog[*ei];
    }
  }
  // ensure that all the variables have been visited
  for (order_t::const_iterator it = o.begin ();
       it != o.end (); ++it)
  {
    if (new_loc[it->first] == 0)
    {
      new_loc[it->first] = it->second;
    }
  }
  
  assert (new_loc.size () == o.size ());
  
  // divide the sums by the nb of adjacent edges to make the average
  for (nb_edge_t::const_iterator it = n.begin ();
       it != n.end (); ++it)
  {
    if (it->second != 0)
    {
      new_loc[it->first] /= it->second;
    }
  }

  return order_from_newloc (new_loc);
}

order_t
new_order_neutral (const std::vector<const edge_t *> & c,
                   const order_t & o)
{
  std::map< std::string, float > new_loc;
  nb_edge_t nb;
  for (std::vector<const edge_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    if ((*it)->cost (o) != 0)
    {
      cog_t cog = (*it)->cog (o);
      for (edge_t::const_iterator ei = (*it)->begin ();
           ei != (*it)->end (); ++ei)
      {
        new_loc[*ei] += cog[*ei];
        nb[*ei]++;
      }
    }
  }
  // ensure that all the variables have been visited
  for (order_t::const_iterator it = o.begin ();
       it != o.end (); ++it)
  {
    if (new_loc[it->first] == 0)
    {
      new_loc[it->first] = it->second;
    }
  }
  
  assert (new_loc.size () == o.size ());
  
  // divide the sums by the nb of adjacent edges to make the average
  for (nb_edge_t::const_iterator it = nb.begin ();
       it != nb.end (); ++it)
  {
    if (it->second != 0)
    {
      new_loc[it->first] /= it->second;
    }
  }
  
  return order_from_newloc (new_loc);
}

int
cost (const std::vector<const edge_t *> & c,
      const order_t & o)
{
  int res = 0;
  for (std::vector<const edge_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    res += (*it)->cost (o);
  }
  return res;
}

order_t
force (const std::vector<const edge_t *> & c,
       const order_t & io)
{
  // \debug
  std::cerr << "initial cost " << cost (c, io) << std::endl;
  
  // compute the number of edges for each label
  nb_edge_t n;
  for (std::vector<const edge_t *>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    for (edge_t::const_iterator ei = (*it)->begin ();
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
    no = new_order (c, res, n);
    if (cost (c, no) >= cost (c, res))
    {
      no = new_order_neutral (c, res);
      if (cost (c, no) >= cost (c, res))
      {
        break;
      }
    }
    /*
    no = new_order (c, res, n, cog);
    if (cost (c, no) >= cost (c, res))
    {
      break;
    }
     */
    res = no;
  }
  
  // \debug
  std::cerr << "final cost " << cost (c, res) << std::endl;
  
  return res;
}
