#ifndef __PTRANSITION__HH__
#define __PTRANSITION__HH__

#include <iosfwd>
#include "its/Naming.hh"
#include "its/petri/Arc.hh"
#include "ddd/Hom.h"
#include "ddd/SHom.h"
#include "its/Ordering.hh"
#include "its/petri/TimeConstraint.hh"

namespace its {

enum Visibility { PUBLIC, PRIVATE };
std::string toString (Visibility v);


class PTransition : public NamedElement {
  Visibility vis_;
  vLabel label_;
public :
  typedef std::vector<Arc> arcs_t;
  typedef arcs_t::const_iterator arcs_it;
private :
  arcs_t arcs_;

  /** private support function */
  enum How {ENABLE, ACTION,DISABLE };
  TimeConstraint clock_;
public :
  /** The name is the unique identifier, while the label is used for synchronizations. */
  PTransition (Label name, Label label, Visibility vis) : NamedElement (name), vis_(vis), label_(label) {};

   // iterator accessor
  arcs_it begin() const { return  arcs_.begin() ; }
  arcs_it end() const { return  arcs_.end() ; }

  Visibility getVisibility() const {
    return vis_;
  }

  Label getLabel () const {
    return label_;
  }
  
  bool isTimed () const { return clock_.isTimed(); }

  bool operator == (const PTransition & other) const {
    return other.getName() == getName() ;
  }

  void addArc (const Arc & a);

  void addClock (int min, int max);

  const TimeConstraint & getClock() const { return clock_ ; }

  void print (std::ostream & os) const ;

};

}

std::ostream & operator<< (std::ostream & os, const its::PTransition & t) ;


#endif // __PTRANSITION__HH__
