#ifndef __INT_EXPRESSION_HH__
#define __INT_EXPRESSION_HH__

#include "util/set.hh"
#include <iostream>
#include <string>
#include <UniqueTable.h>
#include "Variable.hh"

namespace its {

typedef enum { 
  CONST, // an integer constant
  VAR,  // a variable
  PLUS, // nary add
  MULT,  // nary multiplication
  MINUS, // binary minus
  DIV,  // binary division
  MOD, // binary modulo
  POW, // binary power of
  ARRAY // access inside an array
} IntExprType ;

class _IntExpression ;

class Assertion;

// A class to handle integer expressions.
// Relies on concrete a pointer into unique table.
// Use factory to build instances.
class IntExpression {
  // concrete storage
  const _IntExpression * concrete;
  // access to concrete
  friend class _IntExpression;
  // access to _IntExpression* constructor
  friend class VarExpr;
  friend class ArrayVarExpr;
  friend class ConstExpr;
  friend class NaryIntExpr;
  friend class IntExpressionFactory;
  friend class BinaryIntExpr;

  // For factory use
  IntExpression (const _IntExpression * c); 
public :

  IntExpression (int cst);
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
  Label getName () const ;

  /// To determine whether a given variable is mentioned in an expression.
  bool isSupport (const Variable & v) const;

  /// To allow resolution of nested IntExpressions, e.g. array access
  IntExpression getFirstSubExpr () const ;


  // for pretty print
  friend std::ostream & operator<< (std::ostream & os, const IntExpression & e);
};


class Assertion {
  std::pair<IntExpression,IntExpression> mapping ;
public :
  Assertion (const IntExpression & var, const IntExpression & val);
  IntExpression getValue (const IntExpression & v) const ;

  Assertion operator & (const Assertion & other) const;
  
  /// To determine whether a given variable is mentioned in an expression.
  bool isSupport (const Variable & v) const;

  size_t hash() const;
  bool operator== (const Assertion & ) const ;

  // for pretty print
  void print (std::ostream & os) const;
};


typedef d3::multiset<IntExpression>::type NaryParamType ;

class IntExpressionFactory {
  static UniqueTable<_IntExpression> unique;
public :
  static IntExpression  createNary (IntExprType type, NaryParamType params) ;
  static IntExpression  createBinary (IntExprType type, const IntExpression & l, const IntExpression & r) ;
  static IntExpression  createConstant (int v);
  static IntExpression  createVariable (const Variable & v) ;
  static IntExpression  createArrayAccess (const Variable & v, const IntExpression & index) ;
  
  static Assertion createAssertion (const Variable & v,const IntExpression & e);
  static Assertion createAssertion (const IntExpression & v,const IntExpression & e);

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
