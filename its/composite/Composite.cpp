/*
 * Composite.cpp
 *
 *  Created on: 3 déc. 2008
 *      Author: yann
 */
#include "its/composite.hh"
#include <iostream>
#include <algorithm>

using std::find;


namespace its {

const vLabel CState::empty = "";

/** Add an instance to this composite, returns false if instance name already exists,
	 * or if the type name is unknown in the model
	 */
bool Composite::addInstance (Label name, Label type_name, const ITSModel & model) {
	pType type = model.findType(type_name);
	if (type == NULL) {
		std::cerr << "Unknown net type " << type_name
		<< " when handling sub component declaration : subnet " << type_name << " " << name
		<< std::endl;
		return false;
	}
	comps_it it = findName( name, comps_ );
	if (it != comps_.end() ) {
		std::cerr << "Duplicate subnet instance name " << name
		<< " when handling subnet declaration : subnet " << type_name << " " << name
		<< std::endl;
		return false;
	}
	comps_.push_back(Instance(name,type));
	return true;
}
/** Add a synchronization to this composite.
 * Specify name and type (XOR or AND semantics) */
bool Composite::addSynchronization (Label sname, Label slabel) {
	syncs_it sit = findName(sname,syncs_);
	if (sit != syncs_.end()) {
	  std::cerr << "Trying to add a synchronization of name " << sname << " to type " << getName() << " but this name already corresponds to \n" ;
	  sit->print(std::cerr);
	  std::cerr << std::endl;
	  return false;
	}
	syncs_.push_back(Synchronization(sname,slabel));
	return true;
}
/** Add a part to a synchronization */
bool Composite::addSyncPart (Label sname, Label subnetname, Label tname) {
	syncs_it sync = findName(sname,syncs_);
	if (sync == syncs_.end() ) {
	    std::cerr << "Unknown synchronization " << sname << " when handling addSyncPart("
		      << sname << ","
		      << subnetname << ","
		      << tname << ")\n";
	    return false;
	}
	/** forget about checks here, the label may not be yet defined and the instance obviously is. */
	if (subnetname == "self") {
	  const_cast<Synchronization &>(*sync).addPart (subnetname ,tname);
	  return true;
	}
	// reference to a transition of a subnet
	comps_it subnet = findName(subnetname,comps_);
	if (subnet == comps_.end()) {
	  std::cerr << "Unknown subnet " << subnetname << " when handling addSyncPart("
		    << sname << ","
		    << subnetname << ","
		    << tname << ")\n";
	  return false;
	}
	const labels_t & sublabs = subnet->getType()->getTransLabels();
	labels_it subtrans = find(sublabs.begin(), sublabs.end(), tname);
	if (subtrans == sublabs.end()) {
	  // Special case to avoid complaining about untimed components in a timed composition.
	  if (tname == "elapse") {
	    return false;
	  }
	  std::cerr << "Unknown event " << tname << " when handling addSyncPart("
		    << sname << ","
		    << subnetname << ","
		    << tname << ")\n";
	  return false;
	}
	// All checks look OK.
	// discard const qualifier, due to use of findName
	const_cast<Synchronization &>(*sync).addPart (subnetname ,tname);

	return true;
	
}
/** Set an assignment in a state.
 * Effect is: state(subnet) = substate
 * Will create the state if it does not exist, with default states for other instances. */
bool Composite::updateStateDef (Label state, Label subnet, Label substate) {
	bool ret = false;
	cstates_t::iterator it = cstates_.find(state);
	if (it == cstates_.end()) {
		// create a new state
		CState m;
		it = cstates_.insert (cstates_t::value_type(state,m)).first;
		ret = true;
	}
	// update the marking
	it->second.setSubState(subnet,substate);
	return ret;
}

std::ostream & Composite::print (std::ostream & os) const {
	  os << "Composite " << getName() << " {\n\n";

	  os << "    // subnets\n" ;
	  for (comps_it it = comps_.begin();
	       it != comps_.end() ;
	       ++it) {
	    os << "    " << it->getType()->getName() << " " << it->getName() << " ;\n";
	  }

	  os << "    // nested exposed variables \n";
	  for (exposedvars_it it = exposed_.begin();
	       it != exposed_.end();
	       ++it) {
	    os << "    // " << it->first << " in " << it->second << " \n";
	  }
	  os << "\n";

	  os << "    // states definition\n" ;
	  for (cstates_it it=cstates_.begin();
	       it != cstates_.end();
	       ++it) {
	    os << "    state " << it->first ;
	    it->second.print(os);
	    os << "\n";
	  }

	  os << "    // synchronizations\n";
	  for (syncs_it it = syncs_.begin();
	       it != syncs_.end();
	       ++it ) {
	    it->print(os);
	  }

	  os << "}\n";
	  return os;
}



} // namespace
