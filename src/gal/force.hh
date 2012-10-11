#ifndef FORCE_HH_
#define FORCE_HH_

#include <string>
#include <map>
#include <set>
#include <vector>

typedef std::map< std::string, int > order_t;
typedef std::map< std::string, float > cog_t;

class edge_t {
public:
  edge_t (const std::vector<std::string> & v = std::vector<std::string> ()): data_ (v) {}
  virtual ~edge_t() {}
  typedef std::vector< std::string >::const_iterator const_iterator;
  
  const_iterator begin () const { return data_.begin (); }
  const_iterator end () const { return data_.end (); }
  
  virtual int cost (const order_t &) const = 0;
  virtual cog_t cog (const order_t &) const = 0;
  
protected:
  /// ordered edges
  /// one can sort these vectors when working with unordered edges
  std::vector< std::string > data_;
};

//typedef cog_t (*cog_fun_t) (const edge_t *, const order_t &);
//typedef int (*cost_edge_fun_t) (const edge_t &, const order_t &);

/// the main function of the force heuristic
/// takes a set of constraints (edge_t)
/// an initial ordering
/// a center of gravity function
/// a cost function for an edge
/// note: if an edge has a null cost, the algo can take advantage of it
///       by considering it is a satisfied constraint enforcing nothing
///       this may lead to a better optimization
order_t
force (const std::vector< const edge_t * > &,
       const order_t &);

#endif
