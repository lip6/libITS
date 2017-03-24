#ifndef __CIRCULAR__SYNC__HH__
#define __CIRCULAR__SYNC__HH__

#include <iosfwd>

#include "its/Naming.hh"



namespace its {


  class CircularSync : public NamedElement {
    // label
    vLabel label_;
    // labels of "current" arc
    labels_t labcurr_;
    // labels of "next" arc
    labels_t labnext_;

  public :
    /** A circular sync has a name and label, label "" indicating a private event */
    CircularSync (Label name, Label label): NamedElement(name),label_(label) {};

    /** label of this delegator or "" if it is private event */
    Label getLabel () const { return label_; }

    /** set the "current" instance labels */
    void setCurrentLabels (const labels_t & labels) { labcurr_ = labels; }
    /** set the "next" instance labels */
    void setNextLabels (const labels_t & labels) { labnext_ = labels ; }

    const labels_t & getCurrentLabels () const { return labcurr_; }
    const labels_t & getNextLabels () const { return labnext_; }


    std::ostream & print (std::ostream & os) const ;
  };


}


#endif
