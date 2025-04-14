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

std::pair<bool,size_t> printShortestAttacks (std::string & prefix, const GDDD & d, const VarOrder *varOrder, std::ostream & os, bool onEmptyPath = true) {
  if (d == GDDD::one) {
    os << prefix << "\n";
    return {onEmptyPath,1};
  } else if (d == GDDD::null || d==GDDD::top) {
    return {false,0};
  }
  size_t count = 0;
  GDDD without = GDDD::null;
  for (const auto & pair : d) {
    if (pair.first == 0) {
      without = pair.second;
      // attack not using this attack vector
      std::streampos pos = prefix.size();
      auto [leftEmpty,lcount] = printShortestAttacks(prefix, pair.second, varOrder, os, true);
      prefix.resize(pos);
      count += lcount;
      if (leftEmpty) {
        return {true,count};
      }
    } else {
      // should be 1
      assert (pair.first == 1);
      // drop "Edge_" from start of name
      vLabel vname = varOrder->getLabel(d.variable()).substr(5);

      if (prefix.size() == 0) {
        prefix += vname;
      } else {
        prefix += ", " + vname;
      }
      std::streampos pos = prefix.size();
      auto [rightEmpty,rcount] = printShortestAttacks(prefix, pair.second - without, varOrder, os, false);
      prefix.resize(pos);
      count += rcount;
    }
  }
  return {false,count};
}

} // namespace its
#endif
