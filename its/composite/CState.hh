/*
 * CState.hh
 *
 *  Created on: 3 déc. 2008
 *      Author: yann
 */

#ifndef CSTATE_HH_
#define CSTATE_HH_

#include "its/Naming.hh"

namespace its {

/** A class to represent a composite state.
 */
class CState {
	// static allocated in Composite.cpp as =""
	static const vLabel empty;
	typedef std::map<vLabel,vLabel> assignments_t;
	assignments_t assignments_;
public :
	void setSubState (Label variable, Label value) {
		assignments_.insert(make_pair(variable,value));
	}
	Label getSubState (Label pname) const {
		assignments_t::const_iterator it = assignments_.find(pname);
		if (it == assignments_.end())
			return empty;
		else
			return it->second;
	}
	void print (std::ostream & os) const {
		os << "{ ";
		for (assignments_t::const_iterator it = assignments_.begin();
			it != assignments_.end() ;
			++it ) {
			os << (it->first.c_str()) << " = " << it->second << "; ";
		}
		os << "}";
	}

};


} // namespace

#endif /* CSTATE_HH_ */
