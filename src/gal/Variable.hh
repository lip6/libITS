#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include <string>
#include "Naming.hh"

#define GCC_VERSION (__GNUC__ * 10000 \
                + __GNUC_MINOR__ * 100 \
                   + __GNUC_PATCHLEVEL__)

namespace its {

class Variable {
  std::string name;
public:
  Variable(const std::string & nname):name(nname){};
  const std::string & getName () const { return name; }
  bool operator== (const Variable & v) const {
    return v.name == name;
  }
  size_t hash () const { 
    return d3::util::hash<vLabel>()(name);
  }

};

}


#endif
