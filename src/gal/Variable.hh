#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include <string>
#include "Naming.hh"

#define GCC_VERSION (__GNUC__ * 10000 \
                + __GNUC_MINOR__ * 100 \
                   + __GNUC_PATCHLEVEL__)

namespace its {

class Variable {
  vLabel name;
public:
  Variable(Label nname):name(nname){};
  Label getName () const { return name; }
  bool operator== (const Variable & v) const {
    return v.name == name;
  }
  size_t hash () const { 
    return d3::util::hash<vLabel>()(name);
  }

};

}


#endif
