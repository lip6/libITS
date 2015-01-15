#ifndef __PROPERTY_HH__
#define __PROPERTY_HH__

#include "ITSModel.hh"

namespace its {

  enum PropType { REACH, INVARIANT, NEVER};

  class Property {
    vLabel name;
    vLabel predicate;
    PropType isInv;
  public :
    Property(Label name, Label predicate, PropType isInv) : name(name),predicate(predicate), isInv(isInv) {}
    
    Label getName () const { return name; }
    Label getPred() const { return predicate; }
    PropType getType() const { return isInv; }


  };
  
  void loadProps(Label reachFile, std::vector<Property> & props);



}

#endif
