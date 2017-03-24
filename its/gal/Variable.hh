#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include <string>
#include "its/Naming.hh"

namespace its {

class Variable {
  vLabel name;
  vLabel aname;
  int index;
public:
  Variable(Label nname);
  Label getName () const { return name; }
  bool operator== (const Variable & v) const {
    return v.name == name;
  }
  bool operator!= (const Variable & v) const {
    return v.name != name;
  }
  bool operator<(const Variable &v) const {
    return name < v.name;
  }
  size_t hash () const;
  Label getArrayName () const { 
    return aname; 
  }
  int getIndex () const {
    return index;
  }

  operator vLabel() const { return name; }
};
  
}

std::ostream & operator<< (std::ostream &, const its::Variable &);

#endif
