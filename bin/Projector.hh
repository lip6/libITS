#ifndef __PROJECTOR_H__
#define __PROJECTOR_H__

#include "ddd/Hom.h"
#include "its/Ordering.hh"

namespace its {

class Projector : public StrongHom
{
  const VarOrder *varOrder;
public :
  Projector (const VarOrder *varOrder)
      : varOrder (varOrder)
  {}

  GDDD phiOne() const{return GDDD::one;};

  GHom phi(int var,int val) const {
    Label name = varOrder->getLabel(var);
    // does it start with "Edge_"
    if (name.length() > 5 &&  name.compare(0, 5, "Edge_") == 0) {
      // keep it
      return GHom (var,val,this);
    } else {
      // discard
      return this;
    }
  }

  bool operator==(const StrongHom &s) const {
    Projector* ps = (Projector*)&s;
    return varOrder == ps->varOrder;
  }

  size_t hash() const {
    return (size_t)varOrder;
  }

  _GHom * clone () const {  return new Projector(*this); }


  void print (std::ostream & os) const {
      os << "Projector over Edge_";
  }
};

size_t printShortestAttacks (std::stringstream & prefix, const GDDD & d, const VarOrder *varOrder, std::ostream & os) {
  if (d == GDDD::one) {
    os << prefix << "\n";
    return 1;
  } else if (d == GDDD::null || d==GDDD::top) {
    return 0;
  }
  size_t count = 0;
  GDDD without = GDDD::null;
  for (const auto & pair : d) {
    if (pair.first == 0) {
      without = pair.second;
      // attack not using this attack vector
      std::streampos pos = prefix.tellp();
      count += printShortestAttacks(prefix, pair.second, varOrder, os);
      prefix.seekp(pos);
    } else {
      // should be 1
      assert (pair.first == 1);
      // drop "Edge_" from start of name
      vLabel vname = varOrder->getLabel(d.variable()).substr(5);

      if (prefix.tellp() == 0) {
        prefix << vname;
      } else {
        prefix << ", " << vname;
      }
      std::streampos pos = prefix.tellp();
      count += printShortestAttacks(prefix, pair.second - without, varOrder, os);
      prefix.seekp(pos);
    }
  }
  return count;
}

} // namespace its
#endif
