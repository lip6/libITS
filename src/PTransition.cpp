/*
 * PTransition.cpp
 *
 *  Created on: 29 nov. 2008
 *      Author: yann
 */

#include "PTransition.hh"
// for INFINITY constant definition and clockName
#include "TPNet.hh"


#include <iostream>


namespace its {

std::string toString (Visibility v) {
	return 	(v == PUBLIC)? "public" : "private" ;
}

void PTransition::addArc (const Arc & a) {
  arcs_.push_back(a);
}

  void PTransition::addClock (int min, int max) {
    assert(min <= max || max == -1);
    clock_.eft = min;
    clock_.lft = max;
  }


void PTransition::print (std::ostream & os) const {
	os << "    "
	   << toString(getVisibility())
	   << " transition " << getName() 
	   << " label \"" << getLabel() << "\"";

	if ( isTimed() ) {
	  clock_.print(os) ;
	}
	os << " {\n";

	arcs_it it = arcs_.begin();
	while ( it != arcs_.end() ) {
		os << *it++ << "\n";
	}
	os << "    }\n";
}

} // namespace

std::ostream & operator<< (std::ostream & os, const its::PTransition & t)  {
	t.print(os);
	return os;
}
