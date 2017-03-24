#ifndef __PLACE_HH__
#define __PLACE_HH__

#include <iosfwd>
#include "its/Naming.hh"

namespace its {

class Place : public NamedElement {

public:
  Place (Label name) : NamedElement(name) {}

  bool operator == (const Place & other) const {
    return getName() == other.getName();
  }

};

} // namespace

std::ostream & operator<< (std::ostream & os, const its::Place & p);



#endif // ___PLACE_H
