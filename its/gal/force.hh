#ifndef FORCE_HH_
#define FORCE_HH_

#include <cstdlib>
#include <map>
#include <set>
#include <vector>

/**
 The interface to the FORCE algorithm.
 cf.  FORCE: a fast and easy-to-implement variable-ordering heuristic
      F.A. Aloul, I.L. Markov, K.A. Sakallah
      Proceedings of the 13th ACM Great Lakes symposium on VLSI (2003)
 
 The basic idea is as follows.
 A 'locality constraint' is a set of variables.
 Given a set of locality constraints and an initial ordering on variables,
 FORCE computes a new ordering so as to minimize the span of the constraints.
 The main idea is to consider each locality constraint as a set of springs, one
 connecting each variable to the constraint. The springs are all considered
 equivalent, applying the same force.
 FORCE computes the center of gravity (cog) of each constraint.
 Then each variable is assigned the cog of the cogs of each constraint it
 appears in. This gives a new ordering on variables. This new ordering is better
 if the sum of the spans (the costs) of the constraints is decreased.
 The process is iterated (at most |Var| times to limit complexity).
 
 We refine a bit this model.
 Each constraint can be assigned a different strength, and a different cost.
 We also add a deviation to the constraint, that is a deviation of its center of
 gravity.
 Initially designed to obtain orderings more saturation-friendly, it has not
 proven very efficient.
 
 We also allow a constraint to have different spring strengths.
 Thus, each variable may see a different center of gravity of the constraints.
 This allows to have asymmetric constraints, and is especially useful to avoid
 too much queries in the EquivSplit setting.
 */

/// a type for variables
typedef int var_t;
/// an ordering associates an integer position to each variable
typedef std::map<var_t, int> order_t;
/// a positionning associates a float position to each variable
typedef std::map<var_t, float> pos_t;
/// a 'center of gravity' associates a new ideal position to each variable
typedef std::map<var_t, float> cog_t;

class constraint_t {
public:
  /// constructor
  constraint_t (const std::set<var_t> & v = std::set<var_t> (),
                float w = 1)
  : data_ (v), weight_(w), dev_(0) {}
  /// destructor
  virtual ~constraint_t () {}

  /// iterator API
  typedef std::set<var_t>::const_iterator const_iterator;
  const_iterator begin () const { return data_.begin (); }
  const_iterator end () const { return data_.end (); }
  size_t size () const { return data_.size (); }
  const std::set<var_t> & get_data () const { return data_; }

  /// cost
  virtual float cost (const order_t &) const = 0;
  /// center of gravity
  virtual cog_t cog (const order_t &) const = 0;
  /// weight getter
  float weight () const { return weight_; }
  /// weight setter
  void setWeight (float w) { weight_ = w; }
  
  /// deviation getter
  float dev () const { return dev_; }
  /// deviation setter
  void set_dev (float d) { dev_ = d; }
protected:
  /// the set of variables affected by the constraint
  std::set<var_t> data_;
private:
  /// the weight
  float weight_;
  /// the deviation
  float dev_;
};

/**
 The function calling the FORCE heuristic.
 Takes a set of constraints and an initial ordering.
 Return a new ordering that is supposed to better fit the constraints.
 note:  if an edge has a null cost, the algo can take advantage of it
        by considering it is a satisfied constraint enforcing nothing
        this may lead to a better optimization
 */
order_t
force (const std::vector< const constraint_t * > &,
       const order_t &);

#endif
