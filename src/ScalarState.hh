/*
 * ScalarState.hh
 *
 *  Created on: jan 2009
 *      Author: yann
 */

#ifndef SCALARSTATE_HH_
#define SCALARSTATE_HH_

#include <map>
#include "Naming.hh"

namespace its {

/** A class to represent a scalar set state.
 */
class ScalarState {
  vLabel default_;
	// static allocated in Composite.cpp as =""
	static const vLabel empty;
	typedef std::map<vLabel,size_t> assignments_t;
	assignments_t assignments_;
public :
  typedef assignments_t::const_iterator assignments_it;
  assignments_it begin() const { return assignments_.begin() ; }
  assignments_it end() const { return assignments_.end() ; }

  
  // a scalar state is initialized by positionning the default state of all contained instances
  ScalarState (Label default_substate): default_(default_substate) {}
  // given a size for the scalar set "total", return the number of instances which have the default state
  int getDefaultStateCard (int total) const {
    for (assignments_it it = assignments_.begin() ; it != assignments_.end() ; ++it) {
      if (it->first != default_)
	total -= it->second;
    }
    return total;
  }
  // 

  Label getDefault () const { return default_ ; }
  // sets "card" instances to the state "state", instead of default
  void setSubStateCard (Label state, int card) {
    assignments_.insert(make_pair(state,card));
  }
  // returns the number of instances in a given state
  int getSubStateCard (Label pname) const {
    assignments_t::const_iterator it = assignments_.find(pname);
    if (it == assignments_.end())
      return 0;
    else
      return it->second;
  }
  // returns the list of state labels used in this scalar state
  labels_t getSubstates () const {
    labels_t ret ;
    for (assignments_it it = assignments_.begin() ; it != assignments_.end() ; ++it) {
      if (it->first != default_)
	ret.push_back(it->first);
    }
    ret.push_back(default_);
    return ret;
  }
  // pretty print
  void print (std::ostream & os) const {
    os << "{ ";
    for (assignments_it it = assignments_.begin() ; it != assignments_.end() ; ++it) {
      os << "card(" << (it->first.c_str()) << ") = " << it->second << "; ";
    }
    os << "DEFAULT = " << default_ << "; ";
    os << "}";	
  }

};


} // namespace

#endif /* SCALARSTATE_HH_ */
