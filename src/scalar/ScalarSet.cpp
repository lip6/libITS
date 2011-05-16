/*
 * Composite.cpp
 *
 *  Created on: jan 2009
 *      Author: yann
 */
#include "ScalarSet.hh"
#include <iostream>
#include <algorithm>

using std::find;



namespace its {


/** Add an instance to this composite, returns false if instance name already exists,
	 * or if the type name is unknown in the model
	 */
bool ScalarSet::setInstance (Label name, Label type_name, const ITSModel & model) {
  pType type = model.findType(type_name);
  if (type == NULL) {
    std::cerr << "Unknown net type " << type_name
	      << " when handling scalar set declaration : scalar[" << type_name << "] " << name
	      << std::endl;
		return false;
  }
  comp_ = (Instance(name,type));
  return true;
}
/** Add a synchronization to this composite.
 * Specify name and type (XOR or AND semantics) */
bool ScalarSet::addDelegator (Label sname, Label slabel, bool isALL){
  syncs_it sit = findName(sname,syncs_);
  if (sit != syncs_.end()) {
    std::cerr << "Trying to add a delegator of name " << sname << " to type " << getName() << " but this name already corresponds to \n" ;
    sit->print(std::cerr);
    std::cerr << std::endl;
    return false;
  }
  labels_t sublabels = comp_.getType()->getTransLabels();
  labels_it it = find(sublabels.begin(), sublabels.end(), sname);
  if (it == sublabels.end() ) {
    std::cerr << "Trying to add a delegator of name " << sname << " to type " << getName() << " but this name does not correspond to any public transition of contained type :" << comp_.getType()->getName() << std::endl;
    return false;
  }
  syncs_.push_back(Delegator(sname,slabel,isALL));
  return true;
}

  bool ScalarSet::createStateDef (Label state, Label substate) {
    return createStateDef (state, labels_t(size_,substate));
  }

  bool ScalarSet::createStateDef (Label state, labels_t substates) {
    cstates_t::iterator it = cstates_.find(state);
    if (it == cstates_.end()) {

      labels_t substates_exist = comp_.getType()->getInitStates();
      for (labels_it substate = substates.begin() ; substate != substates.end() ; ++substate) {
	if ( find(substates_exist.begin(), substates_exist.end(), *substate) == substates_exist.end() ) {
	  std::cerr << "Unknown instance state " << *substate << " when attempting to create a Scalar State " << state << std::endl;
	  return false;
	}
      }
      // create a new state
      ScalarState m(substates);
      cstates_.insert (cstates_t::value_type(state,m));
      return true;
    } else {
      std::cerr << "State name " << state << " is already used by existing scalar set state :" ;
      it->second.print(std::cerr);
      std::cerr << " \n when handling createStateDef in type " << getName() << std::endl;
      return false;
    }
  }

std::ostream & ScalarSet::print (std::ostream & os) const {
  os << "ScalarSet " << comp_.getType()->getName() << "[" << size_ << "] " <<  getName() << " {\n\n";
  
  os << "    // instance name \n" ;
  os << "    instanceName " << comp_.getName() << ";\n";

  os << "    // states definition\n" ;
  for (cstates_it it=cstates_.begin();
       it != cstates_.end();
       ++it) {
    os << "    state " << it->first ;
    it->second.print(os);
    os << "\n";
  }

  os << "    // delegators \n";
  for (syncs_it it = syncs_.begin();
       it != syncs_.end();
       ++it ) {
    it->print(os);
  }

  os << "}\n";
  return os;
}



} // namespace
