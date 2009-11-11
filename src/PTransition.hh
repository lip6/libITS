#ifndef __PTRANSITION__HH__
#define __PTRANSITION__HH__

#include <iosfwd>
#include "Naming.hh"
#include "Arc.hh"
#include "Hom.h"
#include "SHom.h"
#include "Ordering.hh"
#include "TimeConstraint.hh"

namespace its {

enum Visibility { PUBLIC, PRIVATE };
std::string toString (Visibility v);


class PTransition : public NamedElement {
  Visibility vis_;
public :
  typedef std::vector<Arc> arcs_t;
  typedef arcs_t::const_iterator arcs_it;
private :
  arcs_t arcs_;

  /** private support function */
  enum How {ENABLE, ACTION,DISABLE };
  TimeConstraint clock_;
public :
  PTransition (Label name, Visibility vis) : NamedElement (name), vis_(vis){};

   // iterator accessor
  arcs_it begin() const { return  arcs_.begin() ; }
  arcs_it end() const { return  arcs_.end() ; }

  Visibility getVisibility() const {
    return vis_;
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
