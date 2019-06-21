#include "ToTransRel.hh"


class DoubleVars : public StrongHom {
public:
  DoubleVars(){};
  ~DoubleVars() {};

  GDDD phiOne() const {
    return GDDD::one;
  }

  GHom phi (int var, int val) const {
    return GDDD ( var+1, val) ^ GDDD(var, val)^ GHom(this);
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
      GSDD oldval =  ((const GSDD &) val);
      return GShom(var, GShom(this) (oldval), this);
    } else if (typeid(val) == typeid(const DDD&)) {
      GDDD oldval = (const DDD &)val;
      DDD newval = toRelHom() (oldval);
      GSDD edge = GSDD(var, newval);
      return edge ^ this;
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

its::State toRelation (const its::State & s, const its::Transition & t) {
	GShom torel = ToTransRel();
	its::State s2 = torel(s);
	s2 = t(s2);
	return s2;
}
