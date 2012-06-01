#include <cassert>
#include <typeinfo>


#include "DDD.h"
#include "DED.h"
#include "ExprHom.hpp"
#include "MLHom.h"

namespace its {

// predeclarations
  MLHom queryExpression (const IntExpression & e, const VarOrder * vo);

  GHom assertion (const Assertion & a, const VarOrder * vo);
  GHom invertExpr (const IntExpression & var,const IntExpression & val,const VarOrder * vo, const GDDD & pot) ;



/// Code to handle invert of IntExpression assignments
/// Returns a two variable high DDD : f(x)->x->1
static GDDD invertExprValues (const PIntExpression & expr, const PIntExpression & var, const GDDD & pot) {
  d3::set<GDDD>::type toret ;
  
  // for each potential value : v
  for (GDDD::const_iterator it = pot.begin() ; it != pot.end() ; ++it) {

    // compute expr(v) : combine with assertion variable $0 = v
    // A priori, expr should have a single variable as "env"
    PIntExpression e = expr & 
      PIntExpressionFactory::createPAssertion(var, 
					      PIntExpressionFactory::createConstant(it->first));

    // e should now be a constant.
    assert (e.getType() == CONST);

    // insert f(x)->x pair
    toret.insert( GDDD(1, e.getValue() , GDDD(0, it->first )));
  }

  return DED::add(toret);
}


// assign an expression to a variable
class _InvertExpr:public StrongHom {
  IntExpression var;
  IntExpression expr;
  const VarOrder * vo;
  GDDD potall;
public:
  _InvertExpr(const IntExpression & varr, const IntExpression & e, const VarOrder * vo, const GDDD & ppot) : var(varr), expr(e), vo(vo), potall(ppot) {
    assert(var.getType() != CONST);
  }
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   

  bool
  skip_variable(int vr) const
  {
    Variable curv = Variable(vo->getLabel(vr));
    bool b =  ! var.isSupport(curv)
      && ! expr.isSupport(curv);
//      std::cerr << "Assignment of:" << var << " = " << expr << std::endl
//  	      << "skips ? "<< b << " var " << vo->getLabel(vr) << std::endl;
    return b;
  }

  GHom phi(int vr, int vl) const {
    Variable curv = Variable(vo->getLabel(vr));

    Assertion assertion = IntExpressionFactory::createAssertion(curv, IntExpressionFactory::createConstant(vl));

    // make sure lhs is as simplified as it can be
    IntExpression v = var;
    if (v.getType() == ARRAY && var.isSupport(curv)) {
      v = var & assertion;
    }

    // if lhs is a single variable or a const array access
    // we have resolved the target variable x
    if ( v.getType() == CONSTARRAY || v.getType()==VAR ) {
      
      // if we are at target variable for assignment
      if ( v.isSupport(curv) ) {

	// If the expression is a constant
	if ( expr.getType() == CONST ) {
	  // Invert of  : y = cte  is   : y = {pot}   & [ y == cte ]
	  if ( vl == expr.getValue() ) {
	    // test [ y == cte ] is validated, return pot
	    std::set<GHom> sum;
	    GDDD pot = computeDomain(vr,potall);
	    for (GDDD::const_iterator it = pot.begin() ; it != pot.end() ; ++it ) {
	      sum.insert ( GHom (vr, it->first) );
	    }
	    return GHom::add(sum);
	  } else {
	    // test [ y == cte] is false => return 0
	    return GDDD::null;
	  }
	}

	// Check if expr is a function of at most "v"
	// This is the case if expr & [v =def 0] is constant
	IntExpression constant =  expr & assertion ;
	
	// Indeed, expr is only a function of at most current var
	if (constant.getType() == CONST) {
	  GDDD pot = computeDomain(vr,potall);
	  GDDD inv = invertExprValues(expr.getExpr(), IntExpressionFactory::createVariable(curv).getExpr(), pot);
	  
	  for (GDDD::const_iterator it = inv.begin() ; it != inv.end() ; ++it ) {
	    if (it->first == vl) {
	      GDDD son = it->second; 
	      std::set<GHom> sum;
	      for (GDDD::const_iterator jt = son.begin() ; jt != son.end() ; ++jt) {
		sum.insert (GHom (vr, jt->first));
	      }
	      return GHom::add(sum);
	    }
	  }
	  // if we reach this point, f(x) can never produce current value
	  return GDDD::null;
	}
	
	// Now we have an expression of other variables, that needs some Query to be resolved.

//   	std::cerr << "Invert Assignment: Solving for : " << var << "=" << expr << std::endl
//   		  << "Still need to resolve :" << v << "=" << constant << std::endl;
//   	std::cerr << std::endl;


	if ( ! expr.isSupport(curv) ) {
// 	  std::cerr << "RHS is not support of lhs, querying for " << expr << std::endl;
	  // Resolve by a query the other variables
	  return MLHom(invertExpr(v, expr, vo, potall), 
		       MLHom(vr,vl,queryExpression(expr,vo)));
	}
// 	std::cerr << "RHS uses LHS in its support, using getSubExprExcept mechanism. " << std::endl;

	// Try to extract a subexpression that does not concern current variable
	IntExpression sub = expr.getSubExprExcept ( IntExpressionFactory::createVariable(curv));

//  	std::cerr << "Querying for subexprexcept (" << curv.getName() << ") in rhs " << std::endl;
//   	std::cerr << "Current var : " << vr  << " Var order : " << vo->getLabel(vr)<< std::endl;
// 	std::cerr << "subexprexcept returns : " << sub  << std::endl;
	

	// Resolve by a query the other variables
	return MLHom(invertExpr(v, expr, vo, potall), 
		     MLHom(vr,vl,queryExpression(sub,vo)));

      } else {
	// So, we are not at lhs target variable, 
	// Thus curv is a support for rhs
	// simplify expr by curv assertion and propagate
	IntExpression e = expr & assertion;
	if (e.isSupport(curv)) {
	  // this can only happen if e contains an unresolved array access (e.g. tab[i])
	  // and curv is a var of that array (e.g. tab[0])

	  // still need to resolve nested rhs
	  return MLHom(invertExpr(v,e,vo,potall),MLHom(vr,vl,queryExpression(e.getFirstSubExpr(),vo)));
	} else {
	  // we can skip down to next variable
	  return GHom(vr,vl, invertExpr(v, expr & assertion, vo, potall));
	}

      }
    } else {
      // lhs is a ARRAY variable access
      // still need to resolve lhs
      return MLHom(invertExpr(v, expr ,vo, potall),
		   MLHom(vr,vl,queryExpression(v.getFirstSubExpr(),vo)));
    }
  }

  size_t hash() const {
    return 100787* (var.hash()^expr.hash()) + potall.hash();
  }

  bool operator==(const StrongHom &s) const {
    _InvertExpr* ps = (_InvertExpr*)&s;
    return var.equals(ps->var) && expr.equals(ps->expr) && potall == ps->potall ;
  }

  _GHom * clone () const {  return new _InvertExpr(*this); }

  GHom compose (const GHom & other) const ;

  GHom invert (const GDDD & potall) const { 
    return assignExpr (var, expr, vo);
  }


  void print (std::ostream & os) const {
    os << "Invert(" ;
    os << var << "=";
    expr.print(os);
    os << ")";
  }

};

GHom invertExpr (const IntExpression & var,const IntExpression & val,const VarOrder * vo, const GDDD & pot) {
    return _InvertExpr(var,val,vo,pot);
}




// assign an expression to a variable
class _AssignExpr:public StrongHom {
  IntExpression var;
  IntExpression expr;
  const VarOrder * vo;
public:
  _AssignExpr(const IntExpression & varr, const IntExpression & e, const VarOrder * vo) : var(varr), expr(e), vo(vo) {
    assert(var.getType() != CONST);
  }
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   

  bool
  skip_variable(int vr) const
  {
    Variable curv = Variable(vo->getLabel(vr));
    bool b =  ! var.isSupport(curv)
      && ! expr.isSupport(curv);
//      std::cerr << "Assignment of:" << var << " = " << expr << std::endl
//  	      << "skips ? "<< b << " var " << vo->getLabel(vr) << std::endl;
    return b;
  }

  GHom phi(int vr, int vl) const {
    Variable curv = Variable(vo->getLabel(vr));

    Assertion assertion = IntExpressionFactory::createAssertion(Variable(vo->getLabel(vr)),IntExpressionFactory::createConstant(vl));

    IntExpression e = expr ;
    if (expr.isSupport(curv)) {
      e = e & assertion;
    }
// Eval is now included as a step of applying assertion 
//    e = e.eval();

    IntExpression v = var;
    if (v.getType() == ARRAY && var.isSupport(curv)) {
      v = var & assertion;
    }
//    v = v.eval();

//     if (! e.equals(expr) || ! v.equals(var)) 
//       std::cerr << "Assignment: Solving : " << var << "=" << expr << std::endl
//   	      << "knowing that :" << vo->getLabel(vr) << "=" << vl << std::endl
// 	      << "i.e. :" << assertion << std::endl
//   	      << " reduced to " << v << "=" << e << std::endl;

    

      if ((v.getType() == VAR && vr == vo->getIndex(v.getName())) 
	  || (v.getType() == CONSTARRAY && v.isSupport(curv))) {
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
    } else if (v.getType() != VAR && v.isSupport(Variable(vo->getLabel(vr)))) {
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

  GHom invert (const GDDD & potall) const { 
    return invertExpr (var, expr, vo, potall);
  }

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
// Eval is now included as a step of applying assertion 
//    e= e.eval();


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
// Eval is now included as a step of applying assertion 
// used to eval the two firt parameters
      return assignExpr((var & ((const _AssertionHom *)c)->getAssertion()),
			(expr & ((const _AssertionHom *)c)->getAssertion()),
			vo);
    else
// Eval is now included as a step of applying assertion 
// used to eval the second parameters
      return assignExpr(var,
			(expr & ((const _AssertionHom *)c)->getAssertion()),
			vo);
      
  } else {
    return _GHom::compose(other);
  }
}

GHom _InvertExpr::compose (const GHom & other) const {
  const _GHom * c = get_concret(other);
  if (typeid(*c) == typeid(_AssertionHom)) {
    if (var.getType() != VAR)
// Eval is now included as a step of applying assertion 
// used to eval the two firt parameters
      return assignExpr((var & ((const _AssertionHom *)c)->getAssertion()),
			(expr & ((const _AssertionHom *)c)->getAssertion()),
			vo);
    else
// Eval is now included as a step of applying assertion 
// used to eval the second parameters
      return assignExpr(var,
			(expr & ((const _AssertionHom *)c)->getAssertion()),
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
// Eval is now included as a step of applying assertion 
//     e = e.eval();

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

    // Eval is now included as a step of applying assertion 
    // used to eval the first parameter of "predicate"

    return predicate((expr & ((const _AssertionHom *)c)->getAssertion()),vo);
  } else {
    return _GHom::compose(other);
  }
}

}
