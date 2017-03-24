

#include "its/scalar/CircularSync.hh"


#include <iostream>


namespace its {


  std::ostream & CircularSync::print (std::ostream & os) const {
    os << "    "
       << " circularSync " << getName() 
       << " label(" << label_ << ") "
       << " cur{" ;
    for (labels_it it = labcurr_.begin() ; it != labcurr_.end() ; /* in loop */ ) {
      os << *it;
      if (++it != labcurr_.end())
	os << ",";      
    }
    os << "} next{" ;
    for (labels_it it = labnext_.begin() ; it != labnext_.end() ; /* in loop */ ) {
      os << *it;
      if (++it != labnext_.end())
	os << ",";      
    }
    os << " ;\n";
    return os;
  }


}


