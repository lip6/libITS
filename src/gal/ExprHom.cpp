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
  IntExpression var;
  IntExpression expr;
  const VarOrder * vo;
public:
  _AssignExpr(const IntExpression & varr, const IntExpression & e, const VarOrder * vo) : var(varr), expr(e), vo(vo) {}
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   

  bool
  skip_variable(int vr) const
  {
    Variable curv = Variable(vo->getLabel(vr));
    bool b =  ! var.isSupport(curv)
      && ! expr.isSupport(curv);
//     std::cerr << "Assignment of:" << var << " = " << expr << std::endl
// 	      << "skips ? "<< b << " var " << vo->getLabel(vr) << std::endl;
    return b;
  }

  GHom phi(int vr, int vl) const {
    Variable curv = Variable(vo->getLabel(vr));

    Assertion assertion = IntExpressionFactory::createAssertion(Variable(vo->getLabel(vr)),IntExpressionFactory::createConstant(vl));

    IntExpression e = expr ;
    if (expr.isSupport(curv)) {
      e = e & assertion;
    }
    e = e.eval();

    IntExpression v = var;
    if (v.getType() != VAR && var.isSupport(curv)) {
      v = var & assertion;
    }
    v = v.eval();

//     if (! e.equals(expr) || ! v.equals(var)) 
//      std::cerr << "Assignment: Solving : " << var << "=" << expr << std::endl
//  	      << "knowing that :" << vo->getLabel(vr) << "=" << vl << std::endl
//  	      << " reduced to " << v << "=" << e << std::endl;

    

    if (v.getType() == VAR && vr == vo->getIndex(v.getName()) ) {
      if (e.getType() == CONST) {
	//	std::cerr << "solved" << std::endl;
	// Constant :
	return GHom(vr, e.getValue());
      } else {
      // still need to resolve.
//   	std::cerr << "Assignment: Solving rhs of : " << var << "=" << expr << std::endl
//   		  << "Still need to resolve :" << e.getFirstSubExpr() << std::endl;
//   	std::cerr << "Current var : " << vr  << " Var order : " << vo->getLabel(vr)<< std::endl;
//   	std::cerr << std::endl;
	return MLHom(assignExpr(v,e,vo),MLHom(vr,vl,queryExpression(e.getFirstSubExpr(),vo)));
      }
    } else if (v.getType() != VAR) {
//   	std::cerr << "Assignment: Solving nested lhs for : " << var << "=" << expr << std::endl
//   		  << "Still need to resolve :" << v << "=" << e << std::endl;
//  	std::cerr << "Querying for  :" << v.getFirstSubExpr() << std::endl;
//   	std::cerr << "Current var : " << vr  << " Var order : " << vo->getLabel(vr)<< std::endl;
//   	std::cerr << std::endl;


      // still need to resolve lhs
      return MLHom(assignExpr(v,e,vo),MLHom(vr,vl,queryExpression(v.getFirstSubExpr(),vo)));
    } else if (e.isSupport(Variable(vo->getLabel(vr)))) {
//   	std::cerr << "Assignment: Solving nested rhs for : " << var << "=" << expr << std::endl
//   		  << "Still need to resolve :" << v << "=" << e << std::endl;
//  	std::cerr << "Querying for  :" << e.getFirstSubExpr() << std::endl;
//   	std::cerr << "Current var : " << vr  << " Var order : " << vo->getLabel(vr)<< std::endl;
//   	std::cerr << std::endl;


      // still need to resolve nested rhs
      return MLHom(assignExpr(v,e,vo),MLHom(vr,vl,queryExpression(e.getFirstSubExpr(),vo)));
    } else {
      return GHom(vr,vl, assignExpr(v,e,vo));
    }
  }
  size_t hash() const {
    return 6619*var.hash()^expr.hash();
  }
  bool operator==(const StrongHom &s) const {
    _AssignExpr* ps = (_AssignExpr*)&s;
    return var.equals(ps->var) && expr.equals(ps->expr);
  }
  _GHom * clone () const {  return new _AssignExpr(*this); }

  GHom compose (const GHom & other) const ;

  void print (std::ostream & os) const {
    os << "Assign(" ;
    os << var << "=";
    expr.print(os);
    os << ")";
  }

};

  GHom assignExpr (const IntExpression & var,const IntExpression & val,const VarOrder * vo) {
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

  HomNodeMap phiOne () const {
    HomNodeMap res;
    // We have an array index out of bounds problem !!
    std::cerr << "Query for expression" << a << " produced an overflow error !"<< std::endl;
    res.add(GHom(GDDD::null), GDDD::null);
    return res;
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

  void print (std::ostream & os) const {
    os << "Assertion(" ;
    ass.print(os);
    os << ")";
  }

};

GHom assertion (const Assertion & e, const VarOrder *vo) {
  return _AssertionHom(e,vo);
}

GHom _AssignExpr::compose (const GHom & other) const {
  const _GHom * c = get_concret(other);
  if (typeid(*c) == typeid(_AssertionHom)) {
    if (var.getType() != VAR)
      return assignExpr((var & ((const _AssertionHom *)c)->getAssertion()).eval(),
			(expr & ((const _AssertionHom *)c)->getAssertion()).eval(),
			vo);
    else
      return assignExpr(var,
			(expr & ((const _AssertionHom *)c)->getAssertion()).eval(),
			vo);
      
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
    std::cerr << "Overflow in Predicate when evaluating ";
    expr.print(std::cerr);
    return GDDD::null;
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
//     if (! ( e == expr) ) 
//        std::cerr << "Predicate: Solving : " << expr << std::endl
//  		<< "knowing that :" << vo->getLabel(vr) << "=" << vl << std::endl
//  		<< " reduced to " << e << std::endl;
    
    if (e.getType() == BOOLCONST) {
      // Constant :
      if (e.getValue()) 
	return GHom(vr,vl);
      else
	return GDDD::null;
    } else {
      // still need to resolve.
      IntExpression tmp = e.getFirstSubExpr();
      if (! tmp.equals(0)){
//	std::cerr << "BoolExpr: Still need to resolve :" << tmp << std::endl;
	return MLHom( predicate(e,vo), MLHom(vr,vl,queryExpression(tmp,vo)));
      } else {
	return GHom(vr,vl, predicate(e,vo));
      }
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

  void print (std::ostream & os) const {
    os << "Predicate(" ;
    expr.print(os);
    os << ")";
  }


};

GHom predicate (const BoolExpression & e, const VarOrder * vo) {
  if (e.getType() == BOOLCONST) {
    // Constant :
    if (! e.getValue()) {
      return GHom(GDDD::null);
    } else {
      return GHom::id;
    }
  }

  return _Predicate(e,vo);
}

GHom _Predicate::compose (const GHom & other) const {
  const _GHom * c = get_concret(other);
  if (typeid(*c) == typeid(_AssertionHom)) {
//     std::cerr << "compose " << expr << " with : " << ((const _AssertionHom *)c)->getAssertion() << std::endl;
//     std::cerr << "results in :" << (expr & ((const _AssertionHom *)c)->getAssertion()).eval() << std::endl;
//     c->print(std::cerr);

    return predicate((expr & ((const _AssertionHom *)c)->getAssertion()).eval(),vo);
  } else {
    return _GHom::compose(other);
  }
}

}
