#ifndef __PBOOL_EXPRESSION_HH__
#define __PBOOL_EXPRESSION_HH__

#include <set>
#include <iostream>
#include <string>
#include <ddd/UniqueTable.h>
#include "its/gal/PIntExpression.hh"
#include "its/gal/Variable.hh"


namespace its {

typedef enum { 
  BOOLCONST, // a boolean constant true or false
  BOOLVAR,  // a variable
  OR, // nary or
  AND,  // nary and
  NOT, // unary not
  EQ,  // ==
  NEQ, // !=
  LT, // <
  GT, // >
  LEQ, // <=
  GEQ,  // >=
  BOOLNDEF // The undefined boolean expression, resulting from arith exceptions or array index out of bounds
} BoolExprType ;


class _PBoolExpression ;

// immutable class for handling boolean expressions.
class PBoolExpression {
    // concrete storage
  const _PBoolExpression * concrete;
  // access to concrete or to allow return "this" from derived boolexpr
  friend class _PBoolExpression;
  friend class BoolConstExpr;
  friend class BoolNDefExpr;
  friend class PBoolExpressionFactory;
  friend class NaryBoolExpr;
  friend class BinaryBoolComp;
  friend class NotExpr;
  // For factory use
  PBoolExpression (const _PBoolExpression * c); 

public :

  // copy constructor
  PBoolExpression (const PBoolExpression & other);
  // assignment
  PBoolExpression & operator= (const PBoolExpression & other);
  // destructor (does not reclaim memory). Use PBoolExpressionFactory::garbage for that.
  ~PBoolExpression();

  // comparisons for set/hash storage (based on unique property of concrete).
  bool operator== (const PBoolExpression & other) const ;
  bool operator< (const PBoolExpression & other) const ;
  size_t hash () const;

  // Interpret an integer expression, as a boolean : compare to 1
  PBoolExpression (const PIntExpression & expr) ;

  
  BoolExprType getType() const ;
  // member print
  void print (std::ostream & os, const env_t & env) const ;
  
  // an operator to (partially) resolve expressions.
  // replace occurrences of v (if any) by e.
  PBoolExpression operator& (const PAssertion &a) const;
  // basic operators between two expressions.
  // and
  friend PBoolExpression operator&&(const PBoolExpression & l,const PBoolExpression & r);
  // or
  friend PBoolExpression operator||(const PBoolExpression & l,const PBoolExpression & r);
  // not
  PBoolExpression operator! () const ;

  // resolve what can be resolved at this stage. 
  // Result is a constant expression iff. the expression has no more variables.
  PBoolExpression eval () const ;

  PBoolExpression pushNegations () const;

  /// only valid for CONST expressions
  /// use this call only in form : if (e.getType() == BOOLCONST) { int j = e.getValue() ; ...etc }
  /// Exceptions will be thrown otherwise.
  bool getValue () const ;

  // for public convenience
  friend std::ostream & operator<< (std::ostream & os, const PBoolExpression & e);

  /// To determine whether a given variable is mentioned in an expression.
  bool isSupport (int varIndex, int tabIndex) const;
  /// To get all the variables occuring in the expression
  /// Sets to true mark[i] if the expression uses variable of index i
  void getSupport(bool * const mark) const;

  // rewrite variable indexes using given correspondence table.
  // var x(i) becomes x(newindexes[i]) 
  typedef std::vector<int> indexes_t;
  PBoolExpression reindexVariables (const indexes_t & newindexes) const;


  /// To handle nested expressions (e.g. array access). Returns the constant 0 if there are no nested expressions.
  PIntExpression getFirstSubExpr () const;

  /// To handle nested expressions (resolution in invert procedure). Returns a WrapBoolExpr(this) if there are no appropriate sub expr.
  PIntExpression getSubExprExcept (int,int) const;
  
  void accept(class PBoolExprVisitor *) const;
};

// binary comparisons
PBoolExpression  operator==(const PIntExpression & l, const PIntExpression & r) ;
PBoolExpression  operator!=(const PIntExpression & l, const PIntExpression & r) ;
PBoolExpression  operator< (const PIntExpression & l, const PIntExpression & r) ;
PBoolExpression  operator> (const PIntExpression & l, const PIntExpression & r) ;
PBoolExpression  operator>=(const PIntExpression & l, const PIntExpression & r) ;
PBoolExpression  operator<=(const PIntExpression & l, const PIntExpression & r) ;


typedef d3::set<PBoolExpression>::type NaryPBoolParamType ;

class PBoolExpressionFactory {
  static UniqueTable<_PBoolExpression> & unique ();
  // helper function to fuse internal AND/OR
  static void fuse_internals (BoolExprType, NaryPBoolParamType &);

public :
  // and and or of boolean expressions
  static PBoolExpression createNary (BoolExprType type, NaryPBoolParamType &params) ;
  // and and or, delegates call to Nary version for convenience
  static PBoolExpression  createBinary (BoolExprType type, const PBoolExpression & l, const PBoolExpression & r) ;
  // not !
  static PBoolExpression createNot  (const PBoolExpression & e) ;
  // a boolean constant T or F
  static PBoolExpression createConstant (bool b);

  // An undefined boolean value
  static PBoolExpression createNDef ();

  // a comparison (==,!=,<,>,<=,>=) between two integer expressions
  static PBoolExpression createComparison (BoolExprType type, const PIntExpression & l, const PIntExpression & r) ;

  static void destroy (_PBoolExpression * e);
  static void printStats (std::ostream &os);
};


} // namesapce its

/**************  administrative trivia. *********************************/
/******************************************************************************/
namespace d3 { namespace util { 
  template<>
  struct hash<its::PBoolExpression> {
    size_t operator()(const its::PBoolExpression &g) const{
      return g.hash(); 
    }
  };

  template<>
  struct equal<its::PBoolExpression> {
    bool operator()(const its::PBoolExpression &g1,const its::PBoolExpression &g2) const{
      return g1==g2;
    }
  };
} }


namespace std {
  template<>
  struct less<its::PBoolExpression> {
    bool operator()(const its::PBoolExpression &g1,const its::PBoolExpression &g2) const{
      return g1<g2;
    }
  };
}


#endif
