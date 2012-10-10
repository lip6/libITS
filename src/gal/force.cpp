#include "force.hh"

#include <vector>
#include <algorithm>

#include <assert.h>

// \todo parameterize \a force by passing the cog function as a parameter

int
edge_cog (const edge & e, const order & o)
{
  int e1 = o.find (e.first)->second;
  int e2 = o.find (e.second)->second;
  if (e1 < e2)
    return e1;
  return e2;
}

int
cost (const std::set<edge> & c, const order & o)
{
  int res = 0;
  for (std::set<edge>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    int e1 = o.find (it->first)->second;
    int e2 = o.find (it->second)->second;
    res += (e1 < e2) ? 0 : 1;
  }
  return res;
}

// utility function for sorting according to the second component of the pairs
bool
second_sort (const std::pair<std::string,float> & a, const std::pair<std::string,float> & b)
{
  return a.second < b.second;
}

typedef std::map< std::string, int > nb_edge;

order
new_order (const std::set<edge> & c, const order & o, const nb_edge & n)
{
  std::map< std::string, float > new_loc;
  for (std::set<edge>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    int cog = edge_cog (*it, o);
    new_loc[it->first] += cog;
    new_loc[it->second] += cog;
  }
  // ensure that all the variables have been visited
  for (order::const_iterator it = o.begin ();
       it != o.end (); ++it)
  {
    if (new_loc[it->first] == 0)
    {
      new_loc[it->first] = it->second;
    }
  }
  
  assert (new_loc.size () == o.size ());
  
  // divide the sums by the nb of adjacent edges to make the average
  for (nb_edge::const_iterator it = n.begin ();
       it != n.end (); ++it)
  {
    if (it->second != 0)
    {
      new_loc[it->first] /= it->second;
    }
  }
  
  // and now sort
  std::vector< std::pair< std::string, float > > tmp (new_loc.begin (), new_loc.end ());
  std::sort (tmp.begin (), tmp.end (), second_sort);
  
  order res;
  int i = 0;
  for (std::vector< std::pair< std::string, float > >::const_iterator it = tmp.begin ();
       it != tmp.end (); ++it)
  {
    res[it->first] = i++;
  }
  assert (res.size () == o.size ());
  return res;
}

order
force (const std::set<edge> & c, const order & o)
{
  // \todo optimize everything
  // e.g. the cost of the new order can be computed by \a new_order (not sure)
  
  // compute the number of edges for each label
  nb_edge n;
  for (std::set<edge>::const_iterator it = c.begin ();
       it != c.end (); ++it)
  {
    n[it->first]++;
    n[it->second]++;
  }
  
  order res = o;
  order old = o;
  // arbitrarily fixed maximal number of iterations
  size_t limit = o.size ();
  do
  {
    old = res;
    res = new_order (c, old, n);
  } while ( (limit-- != 0) && (cost (c, res) < cost (c, old)) );
  return res;
}
