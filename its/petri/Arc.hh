#ifndef __ARC__HH__
#define __ARC__HH__

#include <iosfwd>
#include <cassert>
#include "its/Naming.hh"

namespace its {

  enum ArcType { INPUT, OUTPUT, INHIBITOR, RESET, TEST };
vLabel toString (ArcType at);


class ArcVal {
  vLabel place;
  int val;
public :
  ArcVal(Label p, int v): place(p),val(v) {}

  int getValuation() const {
    return val;
  }

  Label getPlace() const {
    return place;
  }
};


class Arc {
  // to support hyperarcs
public :
  typedef std::vector<ArcVal> places_t;
  typedef places_t::const_iterator places_it;
private :
  places_t places_;
  ArcType type_;
public :
  Arc ( ArcType at): type_(at) {}

  Arc ( ArcType at, Label place, int valuation)
    : type_(at)
  {
    addPlace(place,valuation);
  }

  // iterator accessor
  places_it begin() const { return  places_.begin() ; }
  places_it end() const { return  places_.end() ; }

  void addPlace (Label place, int valuation) {
    assert (place != "");
    places_.push_back(ArcVal(place,valuation));
  }

  ArcType getType () const { return type_; }

  void print (std::ostream & os) const ;
};

} // namespace

std::ostream & operator<< ( std::ostream & os, const its::Arc & a) ;

#endif // __ARC__HH__
