#include "ToTransRel.hh"


class DoubleVars : public StrongHom {
public:
  DoubleVars(){};
  ~DoubleVars() {};

  GDDD phiOne() const {
    return GDDD::one;
  }

  GHom phi (int var, int val) const {
    return GHom ( var+1, val)& GHom(var, val, this);
  }

  bool operator== (const StrongHom & o) const {
    return true;
  }

  size_t hash () const {
    return 342661;
  }

  _GHom * clone() const{
    return new DoubleVars();
  }
};

GHom toRelHom () {
  return DoubleVars();
}

class ToTransRel : public StrongShom {
public :
  ToTransRel() {}
  ~ToTransRel() {};
  GSDD phiOne() const {
    return GSDD::one;
  }
  GShom phi (int var, const DataSet &val) const {
    // Used to work for referenced DDD
    if (typeid(val) == typeid(const GSDD &) ) {
      return GShom(var, GShom(this) ((const GSDD &)val), this);
    } else if (typeid(val) == typeid(const DDD&)) {
      DDD newval = toRelHom() ((const DDD &)val);
      return GShom(var, newval, this);
    } else {
      throw "bad type on edge";
    }
  }
  bool operator== (const StrongShom &h) const {
    return true;
  }
  size_t hash() const {
    return 65327;
  }
  _GShom * clone() const {
    return new ToTransRel();
  }
};

its::State getTransRel (const its::ITSModel & m) {

  GShom torel = ToTransRel();
  its::State reach = m.computeReachable();
  its::Transition next = m.getNextRel();
  reach = torel(reach);
  reach = next(reach);
  return reach;
}
