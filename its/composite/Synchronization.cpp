
/*
 * Synchronization.cpp
 *
 *  Created on: 3 d�c. 2008
 *      Author: yann
 */
#include "its/composite/Synchronization.hh"
#include "its/composite/CompositeType.hh"

namespace its {

std::ostream & Synchronization::print (std::ostream & os) const {
	 os << "    "
	    << " synchronization " << getName() << " label \"" << getLabel() << "\" {\n";
	 for (parts_it it = begin(); it != end();  ++it ) {
	   os << "      " ;
	   os << it->first;
	   os << ".\"" << it->second << "\" ;\n";
	 }
	 os << "    }\n";
	 return os;
}



} // namespace
