#include "Variable.hh"

#include "util/hash_support.hh"

#include <stdio.h>

namespace its {

Variable::Variable(Label nname):name(nname) {
  size_t pos = name.find_last_of('[');
  aname = name.substr(0,pos);
  vLabel nstr = name.substr(pos+1, name.find_last_of(']'));
  index=-1;
  sscanf(nstr.c_str(), "%d", &index);
}

size_t Variable::hash () const { 
  return d3::util::hash<vLabel>()(name);
}

}

std::ostream & operator<< (std::ostream & os, const its::Variable & v) {
  os << v.getName ();
  return os;
}
