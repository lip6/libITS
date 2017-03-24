#ifndef __INT_EXPRESSION_HH__
#define __INT_EXPRESSION_HH__

#include "its/gal/PIntExpression.hh"
#include <map>

namespace its {

  // predeclaration for cross includes
  class BoolExpression;
  

class Assertion;

// A class to handle integer expressions.
// Relies on concrete a pointer into unique table.
// Use factory to build instances.
class IntExpression {
  // concrete storage
  const class _IntExpression * concrete;

  // access to concrete
  friend class _IntExpression;
  friend class _BoolExpression;
  // for private ctor access
  friend class IntExpressionFactory;
  friend class BoolExpressionFactory;
  // For factory use
  IntExpression (const _IntExpression * c); 
public :

  IntExpression (int cst=0);
  IntExpression (const Variable &var);
  // copy constructor
  IntExpression (const IntExpression & other);
  // assignment
  IntExpression & operator= (const IntExpression & other);
  // destructor (does not reclaim memory). Use IntExpressionFactory::garbage for that.
  ~IntExpression();


  // comparisons for set/hash storage (based on unique property of concrete).
  // uses equals and less because operator == and < are overloaded to produce BoolExpression
  bool equals (const IntExpression & other) const ;
  bool less (const IntExpression & other) const ;
  size_t hash () const;
  
  // return the type of an expression.
  IntExprType getType() const;
  // return the supporting parametric expression
  const class PIntExpression  getExpr() const ;
  // return the environment
  const env_t & getEnv() const;

  // member print
  void print (std::ostream & os) const ;
  
  // basic operators between two expressions.
  // nary
  friend IntExpression operator+(const IntExpression & l,const IntExpression & r) ;
  friend IntExpression operator*(const IntExpression & l,const IntExpression & r) ;

  // binary
  friend IntExpression operator-(const IntExpression & l, const IntExpression & r) ;
  friend IntExpression operator/(const IntExpression & l, const IntExpression & r) ;
  friend IntExpression operator%(const IntExpression & l, const IntExpression & r) ;
  friend IntExpression operator^(const IntExpression & l, const IntExpression & r) ;
  // unary
  friend IntExpression operator-(const IntExpression & l) ;


  IntExpression operator& (const Assertion &a) const;
  IntExpression assert_eval (const Assertion &a) const;

  // resolve what can be resolved at this stage. 
  // Result is a constant expression iff. the expression has no more variables.
  IntExpression eval () const ;

  /// only valid for CONST expressions
  /// use this call only in form : if (e.getType() == CONST) { int j = e.getValue() ; ...etc }
  /// Exceptions will be thrown otherwise.
  int getValue () const ;

  /// only valid for VAR expressions
  /// use this call only in form : if (e.getType() == VAR) { Label name = e.getName() ; ...etc }
  /// Exceptions will be thrown otherwise.
  vLabel getName () const ;

  /// To determine whether a given variable is mentioned in an expression.
  /// The same, but with a full IntExpression (allowing to carry also array accesses). 
  bool isSupport (const IntExpression & v) const;
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const;

  /// To allow resolution of nested IntExpressions, e.g. array access
  IntExpression getFirstSubExpr () const ;

  /// To allow resolution of nested expressions in context of partial function evaluation
  /// Returns a (maximal) sub expression that DOES NOT contain the target expression
  IntExpression getSubExprExcept (const IntExpression & var) const;

  // for pretty print
  friend std::ostream & operator<< (std::ostream & os, const IntExpression & e);
};


class Assertion {
  PAssertion assertion;
  env_t env;
public :
  Assertion ();
  Assertion (const IntExpression & var, const IntExpression & val);
  Assertion (const PAssertion & a, const env_t & env): assertion(a), env(env) {};
  IntExpression getValue (const IntExpression & v) const ;

  Assertion operator & (const Assertion & other) const;
  

  size_t hash() const;
  bool operator== (const Assertion & ) const ;
  bool operator<  (const Assertion & ) const;
  
  /// simple getters
  const PAssertion & getAssertion() const { return assertion; }
  const env_t & getEnv() const { return env ;}

  /// More involved accessor to extract the lhs and rhs
  IntExpression getLeftHandSide () const;
  IntExpression getRightHandSide () const;



  // for pretty print
  void print (std::ostream & os) const;
};
std::ostream & operator<< (std::ostream & os, const Assertion & a) ;



typedef d3::multiset<IntExpression>::type NaryParamType ;

class IntExpressionFactory {
  static UniqueTable<_IntExpression> & unique();
  
  static std::map<std::string, int> & var_names();
  static std::map<Variable, IntExpression> & var_expr();
public :
  static IntExpression  createNary (IntExprType type, const NaryParamType & params) ;
  static IntExpression  createBinary (IntExprType type, const IntExpression & l, const IntExpression & r) ;
  static IntExpression  createUnary (IntExprType type, const IntExpression & p) ;
  static IntExpression  createConstant (int v);
  static IntExpression  createVariable (const Variable & v) ;
  static IntExpression  createArrayAccess (const Variable & v, const IntExpression & index, int limit) ;
  // More low level version, give an int for the variable index in global names (see getVarIndex)
  // and an int for an array access, -1 means simple var access.
  static IntExpression  createVarAccess (int vindex, int index=-1) ;



  /// value is 1 if true or 0 otherwise
  static IntExpression  wrapBoolExpr (const BoolExpression & b);
  
  static IntExpression createIntExpression (const PIntExpression &, const env_t &);

  static Assertion createAssertion (const Variable & v,const IntExpression & e);
  static Assertion createAssertion (const IntExpression & v,const IntExpression & e);
  static Assertion createAssertion (const PAssertion & a,const env_t & env);
  
  static int getVarIndex (Label);
  static Label getVar (int);

  static void printStats (std::ostream &os);

  // following administrative functions are not really for public usage.
  static const _IntExpression * createUnique(const _IntExpression &);
  static void destroy (_IntExpression * e);
};


} // namespace its


/**************  administrative trivia. *********************************/
/******************************************************************************/
namespace d3 { namespace util { 
  template<>
  struct hash<its::IntExpression> {
    size_t operator()(const its::IntExpression &g) const{
      return g.hash(); 
    }
  };

  template<>
  struct equal<its::IntExpression> {
    bool operator()(const its::IntExpression &g1,const its::IntExpression &g2) const{
      return g1.equals(g2);
    }
  };
} }

namespace std {
  template<>
  struct less<its::IntExpression> {
    bool operator()(const its::IntExpression &g1,const its::IntExpression &g2) const{
      return g1.less(g2);
    }
  };
}

#endif
