/*
 * ScalarState.hh
 *
 *  Created on: jan 2009
 *      Author: yann
 */

#ifndef SCALARSTATE_HH_
#define SCALARSTATE_HH_

#include <map>
#include "its/Naming.hh"

namespace its {

/** A class to represent a scalar set state.
 */
class ScalarState {
  typedef labels_t assignments_t;
  assignments_t assignments_;
public :
  typedef assignments_t::const_iterator assignments_it;
  assignments_it begin() const { return assignments_.begin() ; }
  assignments_it end() const { return assignments_.end() ; }

  
  // a scalar state is initialized by positionning the default state of all contained instances
  ScalarState (Label default_substate, int n): assignments_(n,default_substate) {}

  ScalarState (labels_t substates): assignments_(substates) {}

  Label getState(unsigned int i) const { if (i < assignments_.size()) return assignments_[i] ;  else return assignments_[0];}

  // returns the list of state labels used in this scalar state
  labels_t getSubstates () const {
    std::set<vLabel> lab_set;
    for (assignments_it it = assignments_.begin() ; it != assignments_.end() ; ++it) {
	lab_set.insert(*it);
    }
    return labels_t(lab_set.begin(), lab_set.end());
  }

  // pretty print
  void print (std::ostream & os) const {
    os << "{ ";
    int i=0;
    for (assignments_it it = assignments_.begin() ; it != assignments_.end() ; ++it, ++i) {
      os << "instance[" << i << "] = " << *it << "; ";
    }
    os << "}";	
  }

};


} // namespace

#endif /* SCALARSTATE_HH_ */
