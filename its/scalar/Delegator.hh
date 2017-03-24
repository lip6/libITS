#ifndef __DELEGATOR__HH__
#define __DELEGATOR__HH__

#include <iosfwd>

#include "its/Naming.hh"


namespace its {

  class Delegator : public NamedElement {
    // label
    vLabel label_;
    // Delegator type : ALL or ANY
    bool isALL_;
  public :
    /** A Delegator has a name, a visibility and a type ALL or ANY */
    Delegator (Label name, Label label, bool isALL=true)
      : NamedElement(name),label_(label), isALL_(isALL) {};

    /** label of this delegator or "" if it is private event */
    Label getLabel () const { return label_; }

    /** ALL or ANY */
    bool isALL () const { return isALL_ ; }        

    std::ostream & print (std::ostream & os) const ;
  };


} // namespace


#endif
