#include <cassert>
#include <typeinfo>


#include "ExprHom.hpp"
#include "MLHom.h"

namespace its {

// predeclarations
  MLHom queryExpression (const IntExpression & e, const VarOrder * vo);

  GHom assertion (const Assertion & a, const VarOrder * vo);


// assign an expression to a variable
class _AssignExpr:public StrongHom {
  int var;
  IntExpression expr;
  const VarOrder * vo;
public:
  _AssignExpr(int varr, const IntExpression & e, const VarOrder * vo) : var(varr), expr(e), vo(vo) {}
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   

  bool
  skip_variable(int var) const
  {
    return var != this->var 
      && ! expr.isSupport(Variable(vo->getLabel(var)));
  }

  GHom phi(int vr, int vl) const {
    IntExpression e = expr ;
    if (expr.isSupport(Variable(vo->getLabel(vr)))) {
      e = e & IntExpressionFactory::createAssertion(Variable(vo->getLabel(var)),IntExpressionFactory::createConstant(vl));
    }
    e = e.eval();
    if (vr == var) {
      if (e.getType() == CONST) {
	// Constant :
	return GHom(var, e.getValue());
      } else {
      // still need to resolve.
	return MLHom(assignExpr(var,e,vo),MLHom(vr,vl,queryExpression(e,vo)));
      }
    } else {
      return GHom(vr,vl, assignExpr(var,e,vo));
    }
  }
  size_t hash() const {
    return 6619*var^expr.hash();
  }
  bool operator==(const StrongHom &s) const {
    _AssignExpr* ps = (_AssignExpr*)&s;
    return var == ps->var && expr.equals(ps->expr);
  }
  _GHom * clone () const {  return new _AssignExpr(*this); }

  GHom compose (const GHom & other) const ;
};

  GHom assignExpr (int var,const IntExpression & val,const VarOrder * vo) {
  return _AssignExpr(var,val,vo);
}

// a MLHom to handle : a =? b
// initialized by querying for a =? a and resolve until right hand side is constant
class _QueryMLHom : public StrongMLHom {
  IntExpression a;
  IntExpression b;
  const VarOrder * vo;
public :
  _QueryMLHom (const IntExpression & aa, const IntExpression & bb, const VarOrder * vo) : a(aa),b(bb), vo (vo) {}

  bool
  skip_variable(int var) const
  {
    return ! b.isSupport(Variable(vo->getLabel(var)));
  }


  HomHomMap phi (int var,int val) const {
    IntExpression e = b ;
    if (e.isSupport(Variable(vo->getLabel(var)))) {
      e = e & IntExpressionFactory::createAssertion(Variable(vo->getLabel(var)),IntExpressionFactory::createConstant(val));
    }
    e= e.eval();


    GHom homup = assertion(IntExpressionFactory::createAssertion(a,e),vo);
    MLHom homdown = MLHom::id;
    
    if (e.getType() == CONST) {
      // Constant :
      homdown = GHom(var,val,GHom::id);
    } else {
      homdown = MLHom (var,val,queryExpression(e,vo));
    }
    HomHomMap ret;
    ret.add(homup,homdown);
    return ret;
  }

  bool operator== (const StrongMLHom & s) const {
    _QueryMLHom * ps = (_QueryMLHom *)&s;
    return a.equals(ps->a) && b.equals(ps->b);    
  }

  size_t hash() const {
    return 7489*(a.hash()^(b.hash()+1));
  }
  _MLHom * clone () const {  return new _QueryMLHom(*this); }

};

  MLHom queryExpression (const IntExpression & a, const VarOrder* vo) {
  return _QueryMLHom(a,a,vo);
}

// perform varl := varr independently of variable ordering.
class _AssertionHom:public StrongHom {
  Assertion ass;
  const VarOrder * vo;
public:
  _AssertionHom(const Assertion & expr, const VarOrder * vo) : ass(expr),vo(vo) {}
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   

  bool
  skip_variable(int var) const
  {
    return ! ass.isSupport(Variable(vo->getLabel(var)));
  }

  Assertion getAssertion () const { return ass;}

  GHom phi(int, int) const {
    assert(false);
    return GDDD::null;
  }
  size_t hash() const {
    return 7717*ass.hash();
  }
  bool operator==(const StrongHom &s) const {
    _AssertionHom* ps = (_AssertionHom*)&s;
    return ass == ps->ass;
  }
  _GHom * clone () const {  return new _AssertionHom(*this); }
  GHom compose (const GHom & other) const {
    const _GHom * c = get_concret(other);
    if (typeid(*c) == typeid(_AssertionHom)) {
      return _AssertionHom(ass &  ((const _AssertionHom *)c)->getAssertion(),vo);
    } else {
      return _GHom::compose(other);
    }
  }
};

GHom assertion (const Assertion & e, const VarOrder *vo) {
  return _AssertionHom(e,vo);
}

GHom _AssignExpr::compose (const GHom & other) const {
  const _GHom * c = get_concret(other);
  if (typeid(*c) == typeid(_AssertionHom)) {
    return assignExpr(var,(expr & ((const _AssertionHom *)c)->getAssertion()).eval(),vo);
  } else {
    return _GHom::compose(other);
  }
}


class _Predicate:public StrongHom {
  BoolExpression expr;
  const VarOrder * vo;
public:
  _Predicate(const BoolExpression & e, const VarOrder *vo) : expr(e),vo(vo) {}
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   

  bool
  skip_variable(int var) const
  {
    return ! expr.isSupport(Variable(vo->getLabel(var))) ;
  }
  
  GHom phi(int vr, int vl) const {
    BoolExpression e = expr ;
    if (expr.isSupport(Variable(vo->getLabel(vr)))) {
      e = e & IntExpressionFactory::createAssertion(Variable(vo->getLabel(vr)),IntExpressionFactory::createConstant(vl));
    }
    e = e.eval();
    if (e.getType() == BOOLCONST) {
      // Constant :
      if (e.getValue()) 
	return GHom(vr,vl);
      else
	return GDDD::null;
    } else {
      // still need to resolve.
      return GHom(vr,vl, predicate(e,vo));
    }
  }
  size_t hash() const {
    return 16363*expr.hash();
  }
  bool operator==(const StrongHom &s) const {
    _Predicate* ps = (_Predicate*)&s;
    return expr == ps->expr;
  }
  _GHom * clone () const {  return new _Predicate(*this); }
  GHom compose (const GHom & other) const ;
   bool is_selector() const { return true; }

};

GHom predicate (const BoolExpression & e, const VarOrder * vo) {
  return _Predicate(e,vo);
}

GHom _Predicate::compose (const GHom & other) const {
  const _GHom * c = get_concret(other);
  if (typeid(*c) == typeid(_AssertionHom)) {
    return predicate((expr & ((const _AssertionHom *)c)->getAssertion()).eval(),vo);
  } else {
    return _GHom::compose(other);
  }
}

}
