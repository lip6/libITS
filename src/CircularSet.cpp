/*
 * Composite.cpp
 *
 *  Created on: jan 2009
 *      Author: yann
 */
#include "CircularSet.hh"
#include <iostream>

namespace its {

/** Add a circular synchronization to this composite.
     * Specify name (should be unique among circular synchronizations) */
  bool CircularSet::addSynchronization (Label sname, Label slabel) {
    	circs_it sit = findName(sname,circs_);
	if (sit != circs_.end()) {
	  std::cerr << "Trying to add a circular synchronization of name " << sname << " to type " << getName() << " but this name already corresponds to \n" ;
	  sit->print(std::cerr);
	  std::cerr << std::endl;
	  return false;
	}
	circs_.push_back(CircularSync(sname,slabel));
	return true;
  }


/** Add a part to a synchronization */
  bool CircularSet::addSyncPart (Label sname, InstanceID subnetname, const labels_t & tnames) {
	circs_it circ = findName(sname,circs_);
	if (circ == circs_.end() ) {
	    std::cerr << "Unknown synchronization " << sname << " when handling addSyncPart("
		      << sname << ","
		      << subnetname << ", [";
	    for (labels_it it = tnames.begin() ; it != tnames.end() ; ) {
	      std::cerr << *it;
	      if (++it != tnames.end())
		std::cerr << ",";
	    }
	    std::cerr <<  "] )\n";
	    return false;
	}
	// reference to a transition of a subnet
	const labels_t & sublabs = getInstance().getType()->getTransLabels();
	for (labels_it tname = tnames.begin() ; tname != tnames.end() ; ++tname ) {
	  labels_it subtrans = find(sublabs.begin(), sublabs.end(), *tname);
	  if (subtrans == sublabs.end()) {
	    std::cerr << "Unknown event " << *tname << " when handling addSyncPart("
		      << sname << ","
		      << subnetname << ","
		      << *tname << ")\n";
	    return false;
	  }
	}
	// All checks look OK.
	// discard const qualifier, due to use of findName
	if (subnetname == CURRENT) {
	  const_cast<CircularSync &>(*circ).setCurrentLabels (tnames);
	} else {
	  const_cast<CircularSync &>(*circ).setNextLabels (tnames);
	}

	return true;
	
}


std::ostream & CircularSet::print (std::ostream & os) const {
  os << "CircularSet [" << comp_.getType()->getName() << "] " <<  getName() << " {\n\n";
  
  os << "    // instance name \n" ;
  os << "    instanceName " << comp_.getName() << ";\n";

  os << "    // states definition\n" ;
  for (cstates_it it=cstates_begin();
       it != cstates_end();
       ++it) {
    os << "    state " << it->first ;
    it->second.print(os);
    os << "\n";
  }

  os << "    // delegators \n";
  for (syncs_it it = syncs_begin();
       it != syncs_end();
       ++it ) {
    it->print(os);
  }

  os << "    // circular syncs \n";
  for (circs_it it = circs_begin();
       it != circs_end();
       ++it ) {
    it->print(os);
  }



  os << "}\n";
  return os;
}



} // namespace
