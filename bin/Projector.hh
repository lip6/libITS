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

class Extend : public StrongHom {
public:
  Extend() {}

  GDDD phiOne() const { return GDDD::one; }

  GHom phi(int var, int val) const {
    if (val == 0) {
      return GHom(var, 0, this) + GHom(var, 1, this);
    } else { // val == 1
      return GHom(var, 1, this);
    }
  }

  bool operator==(const StrongHom &s) const {
    return true; // Framework ensures s is of the same class
  }

  size_t hash() const {
    return 17; // Static prime constant for context-free homomorphism
  }

  _GHom * clone() const { return new Extend(*this); }

  void print(std::ostream &os) const {
    os << "Extend";
  }
};

// Static instance of Extend wrapped as a Hom
static Hom extend = GHom(Extend());


size_t printShortestAttacks(std::string &prefix, const GDDD &d, const VarOrder *varOrder, std::ostream &os) {
  if (d == GDDD::one) {
    // Found a minimal attack
    os << prefix << "\n";
    return 1;
  } else if (d == GDDD::null || d == GDDD::top) {
    return 0;
  }

  size_t count = 0;
  GDDD without = GDDD::null;
  int var = d.variable();

  for (const auto &pair : d) {
    if (pair.first == 0) {
      without = pair.second;
      // Recurse on attacks not using this variable
      std::streampos pos = prefix.size();
      count += printShortestAttacks(prefix, pair.second, varOrder, os);
      prefix.resize(pos);
    } else {
      assert(pair.first == 1);

      // Compute all attacks >= some attack in without
      GDDD extend_without = extend(without);

      // Remove from true branch attacks that are >= some in without
      GDDD to_explore = pair.second - extend_without;

      if (to_explore != GDDD::null) {
        // Get variable name, drop "Edge_"
        std::string vname = varOrder->getLabel(var).substr(5);
        // Append to prefix
        if (prefix.empty()) {
          prefix += vname;
        } else {
          prefix += ", " + vname;
        }
        std::streampos pos = prefix.size();
        count += printShortestAttacks(prefix, to_explore, varOrder, os);
        // Restore prefix
        prefix.resize(pos);
      }
    }
  }
  return count;
}

GDDD extractShortestAttacks(const GDDD &d) {
  // Base cases : terminals
  if (d == GDDD::one || d == GDDD::null || d == GDDD::top) {
    return d;
  }

  int var = d.variable();
  GDDD false_child = GDDD::null;
  GDDD true_child = GDDD::null;

  // Extract false and true children
  for (const auto &pair : d) {
    if (pair.first == 0) {
      false_child = pair.second;
    } else if (pair.first == 1) {
      true_child = pair.second;
    }
  }

  // Compute minimal attacks not using the variable
  GDDD minimal_false = extractShortestAttacks(false_child);
  // Compute all attacks >= some attack in false_child
  GDDD extend_false = extend(false_child);
  // Remove non-minimal attacks from true branch
  GDDD to_explore = true_child - extend_false;
  // Compute minimal attacks using the variable
  GDDD minimal_true = extractShortestAttacks(to_explore);

  return GDDD(var,0,minimal_false) + GDDD(var,1,minimal_true);
}


} // namespace its
#endif
