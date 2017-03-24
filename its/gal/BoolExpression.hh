#ifndef __BOOL_EXPRESSION_HH__
#define __BOOL_EXPRESSION_HH__

#include <set>
#include <iostream>
#include <string>
#include <ddd/UniqueTable.h>
#include "its/gal/IntExpression.hh"
#include "its/gal/PBoolExpression.hh"
#include "its/gal/Variable.hh"


namespace its {


class _BoolExpression ;

// immutable class for handling boolean expressions.
class BoolExpression {
    // concrete storage
  const _BoolExpression * concrete;
  // access to concrete
  friend class _BoolExpression;
  friend class BoolExpressionFactory;

  // For factory use
  BoolExpression (const _BoolExpression * c); 
public :
  // default constructor
    BoolExpression (bool value=false);
  // copy constructor
  BoolExpression (const BoolExpression & other);
  // assignment
  BoolExpression & operator= (const BoolExpression & other);
  // destructor (does not reclaim memory). Use BoolExpressionFactory::garbage for that.
  ~BoolExpression();

  // comparisons for set/hash storage (based on unique property of concrete).
  bool operator== (const BoolExpression & other) const ;
  bool operator< (const BoolExpression & other) const ;
  size_t hash () const;

  // Interpret an integer expression, as a boolean : compare to 1
  BoolExpression (const IntExpression & expr) ;

  // return the supporting parametric expression
  const class PBoolExpression & getExpr() const ;
  // return the environment
  const env_t & getEnv() const;
  // Type of the (root of) the boolean expression
  BoolExprType getType() const ;
  // member print
  void print (std::ostream & os) const ;
  
  // an operator to (partially) resolve expressions.
  // replace occurrences of v (if any) by e.
  BoolExpression operator& (const Assertion &a) const;
  BoolExpression assert_eval (const Assertion &a) const;
  // basic operators between two expressions.
  // and
  friend BoolExpression operator&&(const BoolExpression & l,const BoolExpression & r);
  // or
  friend BoolExpression operator||(const BoolExpression & l,const BoolExpression & r);
  // not
  BoolExpression operator! () const ;

  // resolve what can be resolved at this stage. 
  // Result is a constant expression iff. the expression has no more variables.
  BoolExpression eval () const ;

  // Push negations down onto atoms, resulting in a negation free boolean expression
  BoolExpression pushNegations() const;

  /// only valid for CONST expressions
  /// use this call only in form : if (e.getType() == BOOLCONST) { int j = e.getValue() ; ...etc }
  /// Exceptions will be thrown otherwise.
  bool getValue () const ;

  // for public convenience
  friend std::ostream & operator<< (std::ostream & os, const BoolExpression & e);

  /// To determine whether a given variable is mentioned in an expression.
  /// The same, but with a full IntExpression (allowing to carry also array accesses). 
  bool isSupport (const IntExpression & v) const;

  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const;

  /// To handle nested expressions (e.g. array access). Returns the constant 0 if there are no nested expressions.
  IntExpression getFirstSubExpr () const;
};

// binary comparisons
BoolExpression  operator==(const IntExpression & l, const IntExpression & r) ;
BoolExpression  operator!=(const IntExpression & l, const IntExpression & r) ;
BoolExpression  operator< (const IntExpression & l, const IntExpression & r) ;
BoolExpression  operator> (const IntExpression & l, const IntExpression & r) ;
BoolExpression  operator>=(const IntExpression & l, const IntExpression & r) ;
BoolExpression  operator<=(const IntExpression & l, const IntExpression & r) ;


typedef d3::set<BoolExpression>::type NaryBoolParamType ;

class BoolExpressionFactory {
  static UniqueTable<_BoolExpression> & unique ();
public :
  // and and or of boolean expressions
  static BoolExpression createNary (BoolExprType type, const NaryBoolParamType &params) ;
  // and and or, delegates call to Nary version for convenience
  static BoolExpression  createBinary (BoolExprType type, const BoolExpression & l, const BoolExpression & r) ;
  // not !
  static BoolExpression createNot  (const BoolExpression & e) ;
  // a boolean constant T or F
  static BoolExpression createConstant (bool b);

  // a comparison (==,!=,<,>,<=,>=) between two integer expressions
  static BoolExpression createComparison (BoolExprType type, const IntExpression & l, const IntExpression & r) ;
  
  static BoolExpression createBoolExpression (const PBoolExpression &, const env_t &);

  // following administrative functions are not really for public usage.
  static const _BoolExpression * createUnique(const _BoolExpression &);
  static void destroy (_BoolExpression * e);
  static void printStats (std::ostream &os);
};


} // namespace its

/**************  administrative trivia. *********************************/
/******************************************************************************/
namespace d3 { namespace util { 
  template<>
  struct hash<its::BoolExpression> {
    size_t operator()(const its::BoolExpression &g) const{
      return g.hash(); 
    }
  };

  template<>
  struct equal<its::BoolExpression> {
    bool operator()(const its::BoolExpression &g1,const its::BoolExpression &g2) const{
      return g1==g2;
    }
  };
} }


namespace std {
  template<>
  struct less<its::BoolExpression> {
    bool operator()(const its::BoolExpression &g1,const its::BoolExpression &g2) const{
      return g1<g2;
    }
  };
}


#endif
