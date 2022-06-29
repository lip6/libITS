/*
 * Marking.hh
 *
 *  Created on: 29 nov. 2008
 *      Author: yann
 */

#ifndef MARKING_HH_
#define MARKING_HH_

#include <map>
#include <utility>
#include <iostream>
#include "its/Naming.hh"

namespace its {

/** A minimal representation of a marking.
 *  Any variable not specified is defaulted to 0
 */
class Marking {
	typedef std::map<vLabel,int> marks_t;
	marks_t marks_;
public :
	void setMarking (Label pname, int value) {
		marks_.insert(make_pair(pname,value));
	}
	int getMarking (Label pname) const {
		marks_t::const_iterator it = marks_.find(pname);
		if (it == marks_.end())
			return 0;
		else
			return it->second;
	}
	void print (std::ostream & os) const {
		os << "{ ";
		for (marks_t::const_iterator it = marks_.begin();
			it != marks_.end() ;
			++it ) {
			os << (it->first.c_str()) << " = " << it->second << "; ";
		}
		os << "}";
	}
};

} // namespace

#endif /* MARKING_HH_ */
