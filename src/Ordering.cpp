/*
 * Ordering.cpp
 *
 *  Created on: 2 déc. 2008
 *      Author: yann
 */
#include "Ordering.hh"
#include <cassert>
#include <iostream>

namespace its {

  const int DEFAULT_VAR = 0;

VarOrder::VarOrder (const labels_t & vars) {
	setFromCollection (vars);
}

void VarOrder::setFromCollection (const labels_t & vars) {
	indexes_ = map_t();
	int i=0;
	for ( labels_it it = vars.begin(); it != vars.end() ; ++it ) {
		bool insert = indexes_.insert(make_pair(*it,i++)).second;
		if (! insert) {
		  std::cerr<< "Your collection of variable names is not a set !! Variables are not unique." 
			   << "Found duplicate variable " << *it << std::endl;
		  assert(false);
		}
	}
}

int VarOrder::getIndex (Label l) const {
	map_t::const_iterator it = indexes_.find(l);
	if (it == indexes_.end())
		return -1;
	else
		return it->second;
}

static Variable empty ("");
/** return the label asociated to an index */
const Variable & VarOrder::getLabel (int index) const {
	for (map_t::const_iterator it = indexes_.begin() ; it != indexes_.end(); ++it) {
		if (it->second == index)
			return it->first;
	}
	return empty;
}

  /** Add a single variable to index mapping. 
   * Checks are made to ensure index unicity, statement has no effect if the variable already has an index !!
   * so don't use this to reindex a VarOrder */
  bool VarOrder::addVariable (Label var, int index ) {
    if ( getLabel(index) != empty || getIndex(var) != -1)
      return false;
    indexes_.insert( map_t::value_type(var,index));
    return true;
  }



bool VarOrder::updateOrder (const labels_t & vars) {
	// check variable existence in current vars
	for (labels_it it = vars.begin(); it != vars.end() ; ++it ) {
	  if ( getIndex(*it) == -1) {
	    std::cerr << "Unknown variable :" << *it << " when attempting to set a new variable order." << std::endl;
	    std::cerr << "Expected variables are : " ;
	    this->print(std::cerr);
	    std::cerr << std::endl;
	    return false;
	  }
	}
	setFromCollection(vars);
	return true;
}

  std::ostream &  VarOrder::print (std::ostream & os) const {
    for (size_t i=0 ; i < size() ; ++i) {
      os << getLabel(i).getName () << "\n";
    }
    return os;
  }


}
