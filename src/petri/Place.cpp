#include <iostream>
#include "Place.hh"

using namespace its;

/** pretty printing */
std::ostream & operator<< (std::ostream & os, const its::Place & p) {
  os << "    place " << p.getName() << " ;\n";
  return os;
}
