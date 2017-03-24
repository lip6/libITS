#ifndef PINTEXPR_VISITOR_HH_
#define PINTEXPR_VISITOR_HH_

#include "gal/PIntExpression.hh"
#include "gal/PBoolExpression.hh"


namespace its {

class PIntExprVisitor {
public:
  virtual ~PIntExprVisitor() {}
  
  virtual void visitVarExpr (int) = 0;
  virtual void visitConstExpr (int) = 0;
  virtual void visitNDefExpr () = 0;
  virtual void visitWrapBoolExpr (const class PBoolExpression &) = 0;
  virtual void visitArrayVarExpr (int, const class PIntExpression &) = 0;
  virtual void visitArrayConstExpr (int, const class PIntExpression &) = 0;
  virtual void visitNaryIntExpr (IntExprType, const NaryPParamType &) = 0;
  virtual void visitBinaryIntExpr (IntExprType, const class PIntExpression &, const class PIntExpression &) = 0;
  virtual void visitUnaryIntExpr (IntExprType, const class PIntExpression &) = 0;
};

class PBoolExprVisitor {
public:
  virtual ~PBoolExprVisitor() {}
  
  virtual void visitNaryBoolExpr (BoolExprType, const std::vector<class PBoolExpression> &) = 0;
  virtual void visitBinaryBoolComp (BoolExprType, const class PIntExpression &, const class PIntExpression &) = 0;
  virtual void visitNotBoolExpr (const class PBoolExpression &) = 0;
  virtual void visitBoolConstExpr (bool) = 0;
  virtual void visitBoolNDefExpr () = 0;  
};

} // namespace sym

#endif /// ! PINTEXPR_VISITOR_HH_
