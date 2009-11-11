#include "Arc.hh"
#include <iostream>
#include "Hom_PlaceArcs.hh"
#include "Hom_Basic.hh"

namespace its {

std::string toString (ArcType at) {
  switch (at) {
  case INPUT : return "in" ;
  case  OUTPUT : return "out" ;
  case INHIBITOR : return "inhibitor";
  case RESET : return "reset";
  case TEST : return "test";
  default : return "unknown" ;
  }
}

void Arc::print (std::ostream & os) const {
  os << "      " << toString(type_) << " ";
  for (places_it it = places_.begin(); it != places_.end() ; /* increment done in loop body */) {
  	    const ArcVal & av = *it;
  	    os << av.getPlace() ;
	    if (type_ != RESET) os << "(" << av.getValuation() << ") ";
  	    if (++it != places_.end())
  	    	os << ", ";
  }
  os << " ;";
}


} // namespace

std::ostream & operator<< ( std::ostream & os, const its::Arc & a) {
	a.print(os);
	return os;
}
