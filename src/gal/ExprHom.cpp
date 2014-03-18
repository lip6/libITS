#include <cassert>
#include <typeinfo>


#include "DDD.h"
#include "MemoryManager.h"
#include "DED.h"
#include "AdditiveMap.hpp"
#include "IntExpression.hh"
#include "ExprHom.hpp"
// For hash maps
#include "util/configuration.hh"



namespace its {


  typedef std::pair<its::IntExpression, const its::GalOrder *> CacheKey_t;


}

namespace d3 { namespace util {

  // For keys in caches
  // Specialized version for std::pair of hashable objects.
  template <>
  struct hash<its::CacheKey_t> {
    size_t operator() (const its::CacheKey_t &p)const {
      return p.first.hash() ^ ddd::wang32_hash((size_t)p.second);
    };
  };

  // Specialize template to avoid deep comparing VarOrder instances.
  // Mostly we expect only a few different VarOrder instances throughout the program.
  template <>
  struct equal<const its::GalOrder *> {
    bool operator() (const its::GalOrder *a, const its::GalOrder * b)const {
      return a == b;
    };
  };
  
  template <>
  struct equal<std::pair<its::IntExpression,its::IntExpression> > {
    bool operator() (const std::pair<its::IntExpression,its::IntExpression> & a,
                     const std::pair<its::IntExpression,its::IntExpression> & b) {
      return a.first.equals (b.first) && a.second.equals (b.second);
    }
  };
  
}}

namespace its {

  // Useful Typedefs : for cache values = pair<Info=Assertion,node>
  typedef AdditiveMap<IntExpression, GDDD> InfoNode;
  typedef AdditiveMap<std::pair<its::IntExpression,its::IntExpression>, GDDD> TmpInfoNode;
  typedef InfoNode::const_iterator InfoNode_it;
  typedef TmpInfoNode::const_iterator TmpInfoNode_it;

  // For cache keys : pair <expression to query,node>
  typedef std::pair<IntExpression,GDDD> map_key_type;

  // predeclaration: to invert Expressions
  GHom invertExpr (const IntExpression & var,const IntExpression & val,const GalOrder * vo, const GDDD & pot) ;
  // predeclaration: a function to compute queries (no cache)
  static InfoNode queryEval (const IntExpression & e, const GalOrder * vo, const GDDD & d) ;
  // Caches calls to previous function
  static InfoNode query (const IntExpression & e, const GalOrder * vo, const GDDD & d) ;

  typedef Cache<CacheKey_t, GDDD, InfoNode> QueryCache_t;

} // namespace its

template <>
its::InfoNode 
its::QueryCache_t::eval (const its::CacheKey_t & key, const GDDD & param) const {
  return  queryEval(key.first, key.second, param);
}


namespace its {


  static QueryCache_t & getQueryCache();

  class CacheHook : public GCHook {
  public :
    void preGarbageCollect() {
      getQueryCache().clear();
    }
    void postGarbageCollect() {};

  };


  // the cache
  static QueryCache_t & getQueryCache() {
    static QueryCache_t queryCache = QueryCache_t();
    static bool first = true;
    if (first) {
      MemoryManager::addHook(new CacheHook());
      first = false;
    }
    return queryCache;
  }


  static InfoNode query (const IntExpression & e, const GalOrder * vo, const GDDD & d) {
    return getQueryCache().insert(CacheKey_t(e,vo), d).second;
  }

  static InfoNode queryEval (const IntExpression & e, const GalOrder * vo, const GDDD & d) {
    // shortcuts
    int vr = d.variable();
    const IntExpression & curv = vo->getVar(vr);    
    // The final result
    InfoNode res;
    // To hold elements curently being treated
    TmpInfoNode tosolve;
    // To hold potentially partially solved elements for next iteration
    TmpInfoNode remain;

    // Initialize with the assertion that e=e
    remain.add(std::make_pair (e,e),d);

//    std::cerr << " Query for : " << e << std::endl ;

    // As long as there are elements to treat
    while (remain.begin() != remain.end()) {
      // Put remain in "tosolve" and clear remain
      tosolve = remain;
      remain = TmpInfoNode();
      
      // Solve (at least partially) all elements in "tosolve"
      for (TmpInfoNode_it in=tosolve.begin() ; in != tosolve.end() ; ++in ){
  const std::pair<IntExpression,IntExpression> & ass = in->first;
	const GDDD & node = in->second;

//	std::cerr << "Solving element : " << ass << std::endl;
//	std::cerr << "curv : " << curv.getName() << std::endl;

  IntExpression rr = ass.second;
	// For each edge of the DDD
  GDDD::const_iterator end = node.end();
  for (GDDD::const_iterator it = node.begin() ; it != end ; ++it ) {
    // shortcut to edge value
    int vl = it->first;
    IntExpression r = rr;
    bool r_support = r.isSupport (curv);

    // Simplify the rhs of the current assertion by the info
    // only if necessary
    if (r_support) {
      // An assertion representing this edge information
      Assertion assertion = IntExpressionFactory::createAssertion(
								  curv,
								  IntExpressionFactory::createConstant(vl));
      r = r & assertion ;
    }

	  // If RHS is now a constant, we have finished solving
	  if (r.getType() == CONST) {
	    // add result to final res
	    res.add(r, GDDD(vr,vl,it->second));
	  
	  // Otherwise more resolution of rhs is needed
	  } else {
	    
	    // Do we need to do more resolution at this level of the structure ?
	    // Because we first simplified r by the current edge assertion, 
	    // This can only be true if current var is an array, and r has unresolved ARRAY access to it.
	    if ( r.isSupport(curv) ) {
	      
//	      std::cerr << "Iterating to solve nested expression" << std::endl;

	      // Extract a subexpr from r and query for its value.
	      // TODO : get a nested expression that concerns the current array
        IntExpression sub_expr = r.getFirstSubExpr();
	      InfoNode rhssolved = query (sub_expr, vo, GDDD(vr,vl,it->second));
	      for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		// Add to remain for the next iteration of outer loop 
		// The next iteration will work with a more resolved expression
    remain.add(std::make_pair(e, r & IntExpressionFactory::createAssertion (sub_expr, jt->first)), jt->second );
	      }
	     
	    // If curv is no longer a target of the queried expression, propagate "r" as is to child
	    } else {

//	      std::cerr << "Propagating query :" << r << std::endl;
	      
	      InfoNode rhssolved = query (r, vo, it->second);
	      for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		// Upgrade child result to refer to source expression as LHS, 
		// reconstruct missing edge from child result
		// and add to final result.
		res.add(jt->first, GDDD(vr, vl, jt->second ));
	      }
	      

	    }

	  }
	  
	}
	
      }
      
    }
//    std::cerr << " Query for : " << e << " Finished" << std::endl ;
    return res;
    
  }


  class _Assign : public _GHom {
    IntExpression var;
    IntExpression expr;
    const GalOrder * vo;
    public:
    _Assign(const IntExpression & varr, const IntExpression & e, const GalOrder * vo) : var(varr), expr(e), vo(vo) {
      assert(var.getType() == VAR || var.getType() == ARRAY || var.getType() == CONSTARRAY  );
    }

    
    /* Eval */
    GDDD
    eval(const GDDD &d) const
    {
      if( d== GDDD::null)
	{ return GDDD::null; }
      else if( d == GDDD::one)
	{ 
	  std::cerr << "Assignment ";
	  print(std::cerr);
	  std::cerr << " produced an overflow error !"<< std::endl;
	  return GDDD::null; 
	}
      else if( d== GDDD::top)
	{ 
	  std::cerr << " assign hit Top" << std::endl;
	  return GDDD::top; 
	}
      else
	{
	  int vr = d.variable();
	  const IntExpression & curv = vo->getVar(vr);
	  std::set<GDDD> res;
	  GDDD::const_iterator dend = d.end();
	  for (GDDD::const_iterator it = d.begin() ; it != dend ; ++it ) {
	    // current value
	    int vl = it->first;
      
      bool expr_support = expr.isSupport(curv);
      bool var_support = var.getType() == ARRAY && var.isSupport(curv);

      // Simplify expr by current info
	    IntExpression e = expr;
      // If necessary (array access) try to simplify also lhs
	    IntExpression v = var;
      // only if necessary
      if (expr_support || var_support) {
        // An assertion describing info on current arc: var = val
        Assertion assertion = IntExpressionFactory::createAssertion(curv,IntExpressionFactory::createConstant(vl));
        if (expr_support)
          e = e & assertion;
        if (var_support)
          v = v & assertion;
      }

//	    if (! e.equals(expr) || ! v.equals(var)) 
//	             std::cerr << "Assignment: Solving : " << var << "=" << expr << std::endl
//			       << "knowing that :" << vo->getLabel(vr) << "=" << vl << std::endl
//			       << "i.e. :" << assertion << std::endl
//			       << " reduced to " << v << "=" << e << std::endl;

	    // If the lhs is fully resolved to signify the current variable.
	    // This can be a CONSTARRAY or plain VAR
      if ( v.equals(curv) ) {
// (v.getType() == VAR && vr == vo->getIndex(v.getName())) 
//		|| (v.getType() == CONSTARRAY && v.isSupport(curv))) {

	      // Constant case
	      if (e.getType() == CONST) {
		// If the RHS is also resolved (to a constant)

//			std::cerr << "solved" << std::endl;
		res.insert(GDDD(vr, e.getValue(), it->second));
	      } else {
		
		// So LHS is resolved, but RHS still needs some work.
		// Query for its resolved value, provide current variable value
		// This (current) value may be necessary to solve tab[tab[i]] expressions.
		InfoNode rhssolved = query (e, vo, GDDD(vr,vl,it->second));
		for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		  res.insert( assignExpr(v, jt->first, vo) ( jt->second ));
		}
	      }
	    
	    // The lhs is an unresolved ARRAY access that (may) use the current variable
	    } else if (v.getType() == ARRAY && v.isSupport(curv)) {
	      
	      // Query for the value of a nested expression of lhs
	      // TODO : get a nested expression that concerns the current array
        IntExpression sub_expr = v.getFirstSubExpr();
	      InfoNode rhssolved = query (sub_expr, vo, GDDD(vr,vl,it->second));
	      for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		// Note that we also attempt to simplify rhs with the same subexpr result if possible
		// The recursion works with a more resolved expression
    Assertion ass_tmp = IntExpressionFactory::createAssertion (sub_expr, jt->first);
		res.insert( assignExpr(v & ass_tmp, e & ass_tmp, vo) ( jt->second ));
	      }

	    // The LHS does not concern the current variable
	    // If the RHS still concerns the current variable, 
	    // it is an unresolved array access to current array
	    } else if (e.isSupport(curv)) {

	      // Query for the value of a nested expression of rhs
	      // TODO : get a nested expression that concerns the current array
        IntExpression sub_expr = e.getFirstSubExpr();
	      InfoNode rhssolved = query (sub_expr, vo, GDDD(vr,vl,it->second));
	      for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {

		// The recursion works with a more resolved RHS expression
    res.insert( assignExpr(v, e & IntExpressionFactory::createAssertion(sub_expr,jt->first), vo) ( jt->second ));
	      }

	    // Pure propagation of simplified expressions
	    } else {
	      // Neither lhs nor rhs concern current variable 
	      // (anymore, they were simplified or skip would have taken effect)
	      
	      res.insert(GDDD(vr,vl, assignExpr(v,e,vo) (it->second) ));
	    }

	  } // end foreach arc of current node

	  return DED::add(res);

	} // end non terminal DDD case
    }

    bool
    skip_variable(int vr) const
    {
      const IntExpression & curv = vo->getVar(vr);
      bool b =  ! var.isSupport(curv)
	&& ! expr.isSupport(curv);
      //      std::cerr << "Assignment of:" << var << " = " << expr << std::endl
      //  	      << "skips ? "<< b << " var " << vo->getLabel(vr) << std::endl;
      return b;
    }

    _GHom * clone () const {  return new _Assign(*this); }

    GHom invert (const GDDD & potall) const { 
      //std::cerr << "ERROR : invert not implemented !"<< std::endl;
      // return GHom::id;
      //  TODO
      return invertExpr (var, expr, vo, potall);
    }
    
    size_t hash() const {
      return 6619*var.hash()^expr.hash();
    }

    /* Compare */
    bool operator==(const _GHom &h) const {
      if (const _Assign * other = dynamic_cast<const _Assign *> (&h)) {
	return other->vo == vo && other->var.equals(this->var) && other->expr.equals(expr);
      }
      return false;
    }

    void print (std::ostream & os) const {
      os << "Assign(" ;
      os << var << "=";
      expr.print(os);
      os << ")";
    }
  };



class _Predicate:public _GHom {
  BoolExpression expr;
  const GalOrder * vo;
public:
  _Predicate(const BoolExpression & e, const GalOrder *vo) : expr(e),vo(vo) {}
  
  GDDD phiOne() const {
    std::cerr << "Overflow in Predicate when evaluating ";
    expr.print(std::cerr);
    return GDDD::null;
  }                   

  bool
  skip_variable(int var) const
  {
//   expr.print(std::cerr); 
//   std::cerr << "Skips " << vo->getVar(var) << " ? " << ! expr.isSupport(vo->getVar(var)) << std::endl;
   return ! expr.isSupport(vo->getVar(var)) ;
  }
  
  GDDD eval(const GDDD &d) const {
    if( d== GDDD::null)
      { return GDDD::null; }
    else if( d == GDDD::one)
      { 
	std::cerr << "Predicate ";
	print(std::cerr);
	std::cerr << " produced an overflow error !"<< std::endl;
	return GDDD::null; 
      }
    else if( d== GDDD::top)
      { 	  
	std::cerr << "Predicate hit Top" << std::endl;
	return GDDD::top; 
      }
    else
      {
	// current variable 
	int vr = d.variable();
	const IntExpression & curv = vo->getVar(vr);
	
	// To hold result
	std::set<GDDD> res;
	GDDD::const_iterator dend = d.end();
	for (GDDD::const_iterator it = d.begin() ; it != dend ; ++it ) {
	  // current value
	  int vl = it->first;
    bool expr_support = expr.isSupport(curv);

    // Simplify current expression by current edge.
	  BoolExpression e = expr ;
    // only if necessary
    if (expr_support) {
      // An assertion describing info on current arc: var = val
      Assertion assertion = IntExpressionFactory::createAssertion(curv,IntExpressionFactory::createConstant(it->first));
      if (expr_support)
        e = e & assertion;
    }

    //     if (! ( e == expr) ) 
    //        std::cerr << "Predicate: Solving : " << expr << std::endl
    //  		<< "knowing that :" << vo->getLabel(vr) << "=" << vl << std::endl
    //  		<< " reduced to " << e << std::endl;

	  
	  // Check if we now have a constant : Terminal case.
	  if (e.getType() == BOOLCONST) {
	    // Constant :
	    if (e.getValue()) 
	      // True : return current path
	      res.insert(GDDD(vr,vl,it->second));
	    // else
	    // False : Cut current path from result
	    // Not adding edge to res produces this effect
	  } else {

	    // If we still need current variable : 
	    // this means current var is an array and that expr has nested access to it.
	    if (e.isSupport(curv)) {

	      // still need to resolve.
	      // Extract a subexpr from e and query for its value.
	      // TODO : get a nested expression that concerns the current array
        IntExpression sub_expr = e.getFirstSubExpr();
	      InfoNode rhssolved = query (sub_expr, vo, GDDD(vr,vl, it->second));
	      for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		
		// Allow to recurse with the simplified expression
    res.insert ( predicate (e & IntExpressionFactory::createAssertion (sub_expr,jt->first), vo) (jt->second) );
	      }
	    
	    // If RHS no longer targets curv, we can propagate to child
	    } else {
	      res.insert( GDDD(vr,vl, predicate ( e, vo) (it->second)));
	    }
	  } 
	} // end foreach edge of node

	return DED::add(res);
      } // end non terminal case
  }
    
  size_t hash() const {
    return 16363*expr.hash();
  }

  bool operator==(const _GHom &s) const {
    if (const _Predicate * other = dynamic_cast<const _Predicate *> (&s)) {
      return other->vo == vo && other->expr==this->expr;
    }
    return false;
  }

  _GHom * clone () const {  return new _Predicate(*this); }
  
  bool is_selector() const { return true; }

  void print (std::ostream & os) const {
    os << "Predicate(" ;
    expr.print(os);
    os << ")";
  }

};

GHom predicate (const BoolExpression & e, const GalOrder * vo) {
  if (e.getType() == BOOLCONST) {
    // Constant :
    if (! e.getValue()) {
      return GHom(GDDD::null);
    } else {
      return GHom::id;
    }
  }
  if (e.getType() == BOOLNDEF) {
    //    std::cerr << "Building undefined bool pred " << std::endl;
    return GHom(GDDD::null);
  }

  return _Predicate(e,vo);
}
  
GHom assignExpr (const IntExpression & var,const IntExpression & val,const GalOrder * vo) {
  if (var.getType() == INTNDEF || val.getType() == INTNDEF) {
    //    std::cerr << "Building undefined assign " << std::endl;
    return GHom(GDDD::null);
  }
  return _Assign(var,val,vo);
}



/// Code to handle invert of IntExpression assignments
/// Returns a two variable high DDD : f(x)->x->1
static GDDD invertExprValues (const PIntExpression & expr, const PIntExpression & var, const GDDD & pot) {
  d3::set<GDDD>::type toret ;
  
  // for each potential value : v
  GDDD::const_iterator potend = pot.end();
  for (GDDD::const_iterator it = pot.begin() ; it != potend ; ++it) {

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

#ifdef HASH_STAT
void query_stats () {
  std::cout << std::endl << "Query Cache stats:" << std::endl;
  print_hash_stats (getQueryCache ().get_hits (), getQueryCache ().get_misses (), getQueryCache ().get_bounces ());
}
#endif // HASH_STAT



// assign an expression to a variable
class _InvertExpr:public _GHom {
  IntExpression var;
  IntExpression expr;
  const GalOrder * vo;
  GDDD potall;
public:
  _InvertExpr(const IntExpression & varr, const IntExpression & e, const GalOrder * vo, const GDDD & ppot) : var(varr), expr(e), vo(vo), potall(ppot) {
    assert(var.getType() == VAR || var.getType() == ARRAY || var.getType() == CONSTARRAY  );
  }

  void mark () const {
    potall.mark();
  }
  

  bool
  skip_variable(int vr) const
  {
    const IntExpression & curv = vo->getVar(vr);
    bool b =  ! var.isSupport(curv)
      && ! expr.isSupport(curv);
    //      std::cerr << "Invert Assignment of:" << var << " = " << expr << std::endl
    //  	      << "skips ? "<< b << " var " << vo->getLabel(vr) << std::endl;
    return b;
  }

    /* Eval */
  GDDD
  eval(const GDDD &d) const
  { 
     if( d== GDDD::null)
	{ return GDDD::null; }
      else if( d == GDDD::one)
	{ 
	  std::cerr << "Assignment ";
	  print(std::cerr);
	  std::cerr << " produced an overflow error !"<< std::endl;
	  return GDDD::null; 
	}
      else if( d== GDDD::top)
	{ 
	  std::cerr << " assign hit Top" << std::endl;
	  return GDDD::top; 
	}
      else
	{
	  int vr = d.variable();
	  const IntExpression & curv = vo->getVar(vr);
	  std::set<GDDD> res;
	  GDDD::const_iterator dend = d.end();
	  for (GDDD::const_iterator it = d.begin() ; it != dend ; ++it ) {
	    // current value
	    int vl = it->first;

	    bool var_support = var.getType() == ARRAY && var.isSupport(curv);
	    
	    // Simplify expr by current info
	    IntExpression e = expr;
	    // If necessary (array access) try to simplify also lhs
	    IntExpression v = var;
	    // only if necessary
	    Assertion assertion = IntExpressionFactory::createAssertion(curv,IntExpressionFactory::createConstant(vl));
	    
	    
	    // make sure lhs is as simplified as it can be
	    if (var_support) {
	      // An assertion describing info on current arc: var = val
	      v = v & assertion;
	    }	  

	    

	    // If the lhs is fully resolved to signify the current variable.
	    // This can be a CONSTARRAY or plain VAR
	    if ( v.equals(curv) ) {
	      
	      // If the expression is a constant
	      if ( expr.getType() == CONST ) {
		// Invert of  : y = cte  is   : y = {pot}   & [ y == cte ]
		if ( vl == expr.getValue() ) {

		  // test [ y == cte ] is validated, return pot
		  GDDD pot = computeDomain(vr,potall);
		  for (GDDD::const_iterator jt = pot.begin() ; jt != pot.end() ; ++jt ) {
		    res.insert ( GDDD (vr, jt->first, it->second) );
		  }
		  // skip to next edge of argument ddd
		  continue;
		} else {
		  // test [ y == cte] is false => return 0
		  // f(x) cannot give current value as output given constraints on other variables
		  continue;
		}
	      }
	      
	      std::set<Variable> expr_support = expr.getSupport(); 
	      
	      // Check if expr is a function of at most "x", hence in fact x=f(x)
	      IntExpression e2 = e & assertion;
	      if ( e2.getType() == CONST ) {
		//		std::cerr<< "Indeed, expr is only a function of at most current var" << std::endl;
		
		GDDD pot = computeDomain(vr,potall);
		GDDD inv = invertExprValues(expr.getExpr(), curv.getExpr(), pot);
		
		for (GDDD::const_iterator iit = inv.begin() ; iit != inv.end() ; ++iit ) {
		  if (iit->first == vl) {
		    GDDD son = iit->second; 
		    for (GDDD::const_iterator jt = son.begin() ; jt != son.end() ; ++jt) {
		      res.insert (GDDD (vr, jt->first, it->second));
		    }
		    break;
		  }
		}
		// if we reach this point, f(x) can never produce current value
		// try next edge of argument DDD
		continue;
	      } else {	// We are in the case : x = f(x,y...)	
		// Now we have an expression of other variables, that needs some Query to be resolved.
		
		
		//   	std::cerr << "Invert Assignment: Solving for : " << var << "=" << expr << std::endl
		//   		  << "Still need to resolve :" << v << "=" << constant << std::endl;
		//   	std::cerr << std::endl;
		
		if ( ! expr.isSupport(curv) ) {
		  // 	  std::cerr << "RHS is not support of lhs, querying for " << expr << std::endl;
		  // Resolve by a query the other variables
		  
		  // So, x = f(y,z...)
		  // but f does not depend on x
		  // Query values of F
		  InfoNode rhssolved = query (expr, vo, it->second);
		  for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		    // Invert of  : y = cte  is   : y = {pot}   & [ y == cte ]
		    // By definition of query, assertion's rhs should be a constant.
		    
		    
		    if ( vl == jt->first.getValue() ) {
		      // test [ y == cte ] is validated, return pot
		      GDDD pot = computeDomain(vr,potall);
		      for (GDDD::const_iterator jjt = pot.begin() ; jjt != pot.end() ; ++jjt ) {
			res.insert ( GDDD (vr, jjt->first, jt->second) );
		      }
		      // skip to next edge of argument ddd
		      break;
		    }
		  }
		  // skip to next edge of argument DDD
		  continue ;
		} else {
		  
		  // So, x = f(x, y, z...)
		  // We need to resolve other variables first to fall back on x=f(x) case.

		  IntExpression sub;
//		   	std::cerr << "RHS uses LHS in its support, using getSubExprExcept mechanism. " << std::endl;
//        std::cerr << "Invert Assignment: Solving for : " << var << "=" << expr << std::endl
//        << "current variable is " << curv << std::endl;
		  if (expr.isSupport(var)) {
		    // Try to extract a subexpression that does not concern current variable
		    sub = expr.getSubExprExcept (var);
        if (sub.equals (expr))
        {
//          std::cerr << "subexprexcept (" << var << " on " << expr << " has returned this" << std::endl;
//          std::cerr << "on variable " << curv << std::endl;
          GDDD pot = computeDomain (vo->getIndex (var),potall);
          for (GDDD::const_iterator jjt = pot.begin() ; jjt != pot.end() ; ++jjt ) {
            Assertion asstmp = IntExpressionFactory::createAssertion(var,IntExpressionFactory::createConstant(jjt->first));
            IntExpression e_tmp = expr & asstmp;
            InfoNode rhssolved = query (e_tmp, vo, it->second);
            for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
              if (it->first == jt->first.getValue ())
                res.insert (invertExpr (var, jt->first, vo, potall) (GDDD (vr, vl, jt->second)));
            }
		      }
          continue;
        }
		  } else {
		    sub = expr;
		  }
//		    	std::cerr << "Querying for subexprexcept (" << curv.getName() << ") in rhs " << std::endl;
//		     	std::cerr << "Current var : " << vr  << " Var order : " << vo->getLabel(vr)<< std::endl;
//		   	std::cerr << "subexprexcept returns : " << sub  << std::endl;
		    
		  // Query values of sub
		  InfoNode rhssolved = query (sub, vo, it->second);
		  for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		    // Invert of  : y = cte  is   : y = {pot}   & [ y == cte ]
		    // By definition of query, assertion's rhs should be a constant.
		    
		    // The recursion works with a more resolved RHS expression
		    res.insert( invertExpr(v, e & IntExpressionFactory::createAssertion(sub,jt->first), vo, potall) ( GDDD(vr, vl, jt->second )));
		  }
		  // skip to next edge of argument DDD
		  continue ;
		  
		}
	      } // closes other cases of rhs.
	    } else { // curv!= lhs

	      // lhs is not the target var, or is not fully resolved
	      if (v.isSupport(curv) ) { 
		// it should be the case then that v.getType()==ARRAY
		// so it is not fully resolved
		// Make (strong) hypothesis that index function does not depend on previous value
		IntExpression sub_expr = v.getFirstSubExpr();
		InfoNode rhssolved = query (sub_expr, vo, GDDD(vr,vl,it->second));
		for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		  // Note that we also attempt to simplify rhs with the same subexpr result if possible
		  // The recursion works with a more resolved expression
		  Assertion ass_tmp = IntExpressionFactory::createAssertion (sub_expr, jt->first);
		  // simplification of e is legal since we have postulate that sub_expr does not depend on previous value of x
		  res.insert( invertExpr(v & ass_tmp, e & ass_tmp, vo, potall) ( jt->second ));
		}
		continue;
	      } else {
		// So lhs does not depend in any way on current variable
		IntExpression e = expr & assertion;
//          std::cerr << "Invert Assignment: Solving for : " << v << "=" << e << std::endl
//          << "current variable is " << curv << std::endl;
		if (e.isSupport(curv)) {
		  // this can only happen if e contains an unresolved array access (e.g. tab[i])
		  // and curv is a var of that array (e.g. tab[0])
		  // Query for the value of a nested expression of lhs
      
      IntExpression tosolve = e;
      if (e.isSupport (v))
      {
        tosolve = e.getSubExprExcept(v);
      }
      
      InfoNode rhssolved = query (tosolve, vo, GDDD(vr,vl,it->second));
      for (InfoNode_it jt = rhssolved.begin() ; jt != rhssolved.end() ; ++jt ) {
		    // Note that we also attempt to simplify rhs with the same subexpr result if possible
		    // The recursion works with a more resolved expression
		    Assertion ass_tmp = IntExpressionFactory::createAssertion (tosolve, jt->first);
		    res.insert( invertExpr(v, e & ass_tmp, vo, potall) ( jt->second ));
      }
      
		  continue;
		} else {
		  // we can skip down to next variable
		  res.insert(GDDD(vr,vl, invertExpr(v, e, vo, potall) (it->second)));
		  continue;
		}
	      }
	    }
	  } // foreach edge of DDD 
	  return DED::add(res);
	} // non terminal case 
  }


  size_t hash() const {
    return 100787* (var.hash()^expr.hash()) + potall.hash();
  }

  bool operator==(const _GHom &s) const {
    _InvertExpr* ps = (_InvertExpr*)&s;
    return ps->vo == vo && var.equals(ps->var) && expr.equals(ps->expr) && potall == ps->potall ;
  }

  _GHom * clone () const {  return new _InvertExpr(*this); }
 
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




GHom invertExpr (const IntExpression & var,const IntExpression & val,const GalOrder * vo, const GDDD & pot) {
  if (var.getType () == CONSTARRAY && ! vo->isValidAddress (IntExpressionFactory::getVar (var.getEnv ()[var.getExpr ().getVariable ()]), var.getValue ()))
    return GDDD::null;
  if (var.getType() == INTNDEF || val.getType() == INTNDEF) {
    //    std::cerr << "Building undefined assign " << std::endl;
    return GHom(GDDD::null);
  }

  return _InvertExpr(var,val,vo,pot);
}

/**

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

*/

}
