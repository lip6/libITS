#include "Delegator.hh"

#include <iostream>


namespace its {


  std::ostream & Delegator::print (std::ostream & os) const {
    os << "    "
         << (isALL_?"ALL":"ANY")
       << " delegator " << getName() 
       << " label(" << label_ << ") "
       << " ;\n";
    return os;
  }


}


