#ifndef __PROPERTY_HH__
#define __PROPERTY_HH__

#include "ITSModel.hh"

namespace its {


  class Property {
    vLabel name;
    vLabel predicate;
    bool isInv;
  public :
    Property(Label name, Label predicate, bool isInv) : name(name),predicate(predicate), isInv(isInv) {}
    
    Label getName () const { return name; }
    Label getPred() const { return predicate; }
    bool isInvariant() const { return isInv; }


  };
  
  void loadProps(Label reachFile, std::vector<Property> & props);



}

#endif
