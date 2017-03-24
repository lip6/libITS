#ifndef __PINT_EXPRESSION_HH__
#define __PINT_EXPRESSION_HH__

#include "ddd/util/set.hh"
#include <iostream>
#include <string>
#include <ddd/UniqueTable.h>
#include "its/gal/Variable.hh"

namespace its {
  // predeclaration for cross includes
  class PBoolExpression;

typedef enum { 
  CONST, // an integer constant
  VAR,  // a variable
  PLUS, // nary add
  MULT,  // nary multiplication
  MINUS, // binary minus
  DIV,  // binary division
  MOD, // binary modulo
  POW, // binary power of
  BITAND, // bitwise AND &
  BITOR, // bitwise OR |
  BITXOR, // bitwise XOR ^
  WRAPBOOL, // to evaluate a boolean as 0 or 1
  LSHIFT, // left shift bitwise operator <<
  RSHIFT, // right shift bitwise operator >>
  ARRAY, // access inside an array
  CONSTARRAY, // access to a given cell of an array
  INTNDEF, // An undefined value, typically obtained by array index out of bounds
  BITCOMP // bitwise complement ~
} IntExprType ;

class _PIntExpression ;

class PAssertion;
typedef d3::multiset<class PIntExpression>::type NaryPParamType ;

typedef std::vector<int> env_t;

// A class to handle integer expressions.
// Relies on concrete a pointer into unique table.
// Use factory to build instances.
class PIntExpression {
  // concrete storage
  const _PIntExpression * concrete;
  // access to concrete
  friend class _PIntExpression;
  // access to _PIntExpression* constructor
  friend class VarExpr;
  friend class ArrayVarExpr;
  friend class ArrayConstExpr;
  friend class ConstExpr;
  friend class NDefExpr;
  friend class NaryIntExpr;
  friend class UnaryIntExpr;
  friend class PIntExpressionFactory;
  friend class BinaryIntExpr;
  friend class WrapBoolExpr;

  // For factory use
  PIntExpression (const _PIntExpression * c); 
public :

  PIntExpression (int cst);
  PIntExpression (const Variable &var);
  // copy constructor
  PIntExpression (const PIntExpression & other);
  // assignment
  PIntExpression & operator= (const PIntExpression & other);
  // destructor (does not reclaim memory). Use PIntExpressionFactory::garbage for that.
  ~PIntExpression();


  // comparisons for set/hash storage (based on unique property of concrete).
  // uses equals and less because operator == and < are overloaded to produce PBoolExpression
  bool equals (const PIntExpression & other) const ;
  bool less (const PIntExpression & other) const ;
  size_t hash () const;
  
  // return the type of an expression.
  IntExprType getType() const;
  // member print
  void print (std::ostream & os, const env_t & env) const ;
  
  // basic operators between two expressions.
  // nary
  friend PIntExpression operator+(const PIntExpression & l,const PIntExpression & r) ;
  friend PIntExpression operator*(const PIntExpression & l,const PIntExpression & r) ;

  // binary
  friend PIntExpression operator-(const PIntExpression & l, const PIntExpression & r) ;
  friend PIntExpression operator/(const PIntExpression & l, const PIntExpression & r) ;
  friend PIntExpression operator%(const PIntExpression & l, const PIntExpression & r) ;
  friend PIntExpression operator^(const PIntExpression & l, const PIntExpression & r) ;
  // unary
  friend PIntExpression operator-(const PIntExpression & l) ;


  PIntExpression operator& (const PAssertion &a) const;

  // resolve what can be resolved at this stage. 
  // Result is a constant expression iff. the expression has no more variables.
  PIntExpression eval () const ;

  // rewrite variable indexes using given correspondence table.
  // var x(i) becomes x(newindexes[i]) 
  typedef std::vector<int> indexes_t;
  PIntExpression reindexVariables (const indexes_t & newindexes) const;

  /// only valid for CONST expressions or CONSTARRAY where it returns the index
  /// use this call only in form : if (e.getType() == CONST) { int j = e.getValue() ; ...etc }
  /// Exceptions will be thrown otherwise.
  int getValue () const ;

  /// only valid for VAR expressions or CONSTARRAY where it returns the tab var 
  /// use this call only in form : if (e.getType() == VAR) { Label name = e.getName() ; ...etc }
  /// Exceptions will be thrown otherwise.
  int getVariable () const ;

  /// only valid for Nary expressions (PLUS, MULT)
  /// Exceptions will be thrown otherwise.
  const NaryPParamType & getParams() const ;


  /// To determine whether a given variable is mentioned in an expression.
  bool isSupport (int varIndex, int tabIndex) const;
  /// To get all the variables occuring in the expression
  /// Sets to true mark[i] if the expression uses variable of index i
  void getSupport(bool * const mark) const;

  /// To allow resolution of nested PIntExpressions, e.g. array access
  PIntExpression getFirstSubExpr () const ;

  /// To allow partial resolution of expressions for invert computations. index -1 means any entry in the array (or a scalar var).
  PIntExpression getSubExprExcept (int var, int index) const ;

  void accept(class PIntExprVisitor *) const;
};

class PAssertion {
  std::pair<PIntExpression,PIntExpression> mapping ;
public :
  PAssertion (const PIntExpression & var, const PIntExpression & val);
  PIntExpression getValue (const PIntExpression & v) const ;

  PAssertion operator & (const PAssertion & other) const;
  
  /// To determine whether a given variable is mentioned in an expression.
  bool isSupport (int var, int id) const;

  size_t hash() const;
  bool operator== (const PAssertion & ) const ;
  bool operator<  (const PAssertion & ) const;
  
  /// simple getters
  PIntExpression getFirst() const { return mapping.first; }
  PIntExpression getSecond() const { return mapping.second; }

  // for pretty print
  void print (std::ostream & os, const env_t & env) const;
};



class PIntExpressionFactory {
  friend class IntExpressionFactory;
  static UniqueTable<_PIntExpression> & unique();
public :
  static PIntExpression  createUnary (IntExprType type, const PIntExpression & p) ;
  static PIntExpression  createNary (IntExprType type, const NaryPParamType & params) ;
  static PIntExpression  createBinary (IntExprType type, const PIntExpression & l, const PIntExpression & r) ;
  static PIntExpression  createConstant (int v);
  static PIntExpression  createNDef ();
  static PIntExpression  createVariable (int varIndex) ;
  static PIntExpression  createArrayAccess (int var, const PIntExpression & index, int limit) ;
  /// value is 1 if true or 0 otherwise
  static PIntExpression  wrapBoolExpr (const PBoolExpression & b);

  static PAssertion createPAssertion (const Variable & v,const PIntExpression & e);
  static PAssertion createPAssertion (const PIntExpression & v,const PIntExpression & e);

  static void printStats (std::ostream &os);

  // following administrative functions are not really for public usage.
  static const _PIntExpression * createUnique(const _PIntExpression &);
  static void destroy (_PIntExpression * e);
  
  static void pstats ();
};


} // namespace its


/**************  administrative trivia. *********************************/
/******************************************************************************/
namespace d3 { namespace util { 
  template<>
  struct hash<its::PIntExpression> {
    size_t operator()(const its::PIntExpression &g) const{
      return g.hash(); 
    }
  };

  template<>
  struct equal<its::PIntExpression> {
    bool operator()(const its::PIntExpression &g1,const its::PIntExpression &g2) const{
      return g1.equals(g2);
    }
  };
} }

namespace std {
  template<>
  struct less<its::PIntExpression> {
    bool operator()(const its::PIntExpression &g1,const its::PIntExpression &g2) const{
      return g1.less(g2);
    }
  };
}

#endif
