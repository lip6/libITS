#ifndef __STATEMENTS_HH__
#define __STATEMENTS_HH__


#include "its/gal/IntExpression.hh"
#include "its/gal/BoolExpression.hh"



namespace its {


enum StatementType {
    ASSIGN,
    SYNCASSIGN,
    SEQ,
    IfThenElse,
    FIXPOINT,
    CHOICE,
    CALL,
    ABORT,
    INCRASSIGN
};


class StatementVisitor {

public :
  virtual ~StatementVisitor() {} 

  virtual void visitAssign (const class Assignment & ass)=0; 
  virtual void visitSequence (const class Sequence & seq)=0; 
  virtual void visitIte (const class Ite & ite)=0; 
  virtual void visitFix (const class FixStatement & loop)=0; 
  virtual void visitCall (const class Call & call)=0; 
  virtual void visitAbort ()=0; 
  virtual void visitIncrAssign (const class IncrAssignment & ass)=0;
  virtual void visitSyncAssign (const class SyncAssignment & ass)=0;
};


class Statement {
protected :
  static void indent (std::ostream & os, int ind=0) {
    for (int i = 0; i < ind ; ++i) {
      os << "  ";
    } 
  } 
  
public : 
  virtual ~Statement() {}
  virtual StatementType getType() const = 0;  
  
  virtual Statement * clone () const = 0;

  virtual void acceptVisitor (StatementVisitor & vis) const = 0;
  /// pretty print
  virtual void print (std::ostream & os, int indent=0) const = 0;

  virtual std::set<Variable> getSupport() const = 0;
};


/** 
 * An assignment is a basic atomic action that updates a single variable of the GAL 
 * based on an expression on the current variable states.
 * e.g. X = Y + ( 2 * X )
*/ 
class Assignment : public Statement {
  // Note : the lhs should be of type Variable or ArrayAccess, ultimately resolved to a variable.
  // It is an error if the lhs resolves to a constant value.
  IntExpression var_;
  // A contrario, the rhs should evaluate to a constant
  IntExpression expr_;
    
  public :
  /// Constructor, by reference since IntExpr are by construction unique
  Assignment (const IntExpression & var, const IntExpression & expr) : var_(var.eval()), expr_(expr.eval()) {}
  /// Returns the left hand side of the Assignment
  const IntExpression & getVariable () const { return var_; }
  /// Returns the right hand side of the Assignment
  const IntExpression & getExpression () const { return expr_; }
  /// pretty print
  void print (std::ostream & os, int ind) const {
    indent(os,ind);
    var_.print(os);
    os <<  " = " ;
    expr_.print(os);    
  }
  
  /// simplifies the current expression by asserting the value of a variable.
 // Assignment operator&(const Assertion &) const;
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const;
  /// equality comparison
  bool operator==(const Assignment &) const;
  /// ordering (for use with sets and maps)
  bool operator< (const Assignment &) const;

  StatementType getType() const { return ASSIGN; }
  
  Statement * clone () const { return new Assignment(*this); }

  void acceptVisitor (class StatementVisitor & vis) const {  vis.visitAssign(*this) ; }    
};

/**
 * A synchronous assignment of several variables.
 * Required for circuits synchronous semantics.
 * E.g.   x := y; y := x;
 *        After this operation done with two simple assignments chained in sequence, x == y and the value of y has not changed.
 *        After this operation done with a synchronous assignment, the values of x and y are swapped.
 */
class SyncAssignment : public Statement {
public:
  using assign_t = std::pair<IntExpression, IntExpression>;
private:
  /// Note: the lhs should ultimately resolve to a variable, and the rhs to a constant.
  std::vector<assign_t> _assignments;

public:
  explicit SyncAssignment (const std::vector<assign_t> &);

  const std::vector<std::pair<IntExpression, IntExpression>> & getAssignments () const { return _assignments; }

  std::set<Variable> getSupport () const;

  StatementType getType () const { return SYNCASSIGN; }

  Statement * clone () const { return new SyncAssignment(*this); }

  void print (std::ostream &os, int ind) const;

  void acceptVisitor (class StatementVisitor & vis) const { vis.visitSyncAssign (*this); }

  SyncAssignment operator& (const its::Assertion &) const;
};

/** 
 * An IncrAssignment is a basic atomic action that updates a single variable of the GAL 
 * based on an expression on the current variable states.
 * e.g. X += Y + ( 2 * Z )
 * There is no DecrAssign : it is simulated by  x -= e;  <=>  x += -e;
*/ 
class IncrAssignment : public Statement {
  // Note : the lhs should be of type Variable or ArrayAccess, ultimately resolved to a variable.
  // It is an error if the lhs resolves to a constant value.
  IntExpression var_;
  // A contrario, the rhs should evaluate to a constant
  IntExpression expr_;
    
  public :
  /// Constructor, by reference since IntExpr are by construction unique
  IncrAssignment (const IntExpression & var, const IntExpression & expr) : var_(var.eval()), expr_(expr.eval()) {}
  /// Returns the left hand side of the IncrAssignment
  const IntExpression & getVariable () const { return var_; }
  /// Returns the right hand side of the IncrAssignment
  const IntExpression & getExpression () const { return expr_; }
  /// pretty print
  void print (std::ostream & os, int ind) const {
    indent(os,ind);
    var_.print(os);
    os <<  " += " ;
    expr_.print(os);    
  }
  
  /// simplifies the current expression by asserting the value of a variable.
 // Assignment operator&(const Assertion &) const;
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const;
  /// equality comparison
  bool operator==(const IncrAssignment &) const;
  /// ordering (for use with sets and maps)
  bool operator< (const IncrAssignment &) const;

  StatementType getType() const { return INCRASSIGN; }
  
  Statement * clone () const { return new IncrAssignment(*this); }

  void acceptVisitor (class StatementVisitor & vis) const {  vis.visitIncrAssign(*this) ; }    
};



/** 
 * A sequence is simply a list of statements, that should be executed in order.
 * Syntactically, they are separated by semi columns ;
 * */ 
class Sequence : public Statement {
public :
  typedef std::vector<Statement *> actions_t;
  typedef actions_t::const_iterator const_iterator;
private :
  actions_t actions;
public :
  /// Constructor, by reference since IntExpr are by construction unique
  Sequence () {}
  
  void operator= (const Sequence & o) {
    if ( &o != this ) {
    for (actions_t::iterator it = actions.begin(); it != actions.end() ; ++it) {
      delete *it;
    }
    actions = actions_t();
    actions.reserve(o.actions.size());
    for (const_iterator it =  o.begin(); it != o.end() ; ++it) {
      actions.push_back( (*it)->clone());
    }

    } 
  }

  Sequence (const Sequence & o) {
    actions.reserve(o.actions.size());
    for (const_iterator it =  o.begin(); it != o.end() ; ++it) {
      actions.push_back( (*it)->clone());
    }
  }
  size_t size () const { return actions.size() ; }
  const_iterator begin() const { return actions.begin(); }
  const_iterator end() const { return actions.end(); }
 
  ~Sequence() {
    for (actions_t::iterator it = actions.begin(); it != actions.end() ; ++it) {
      delete *it;
    }
  }

  void add (const Statement & s) {
    actions.push_back(s.clone());
  }
  void prepend (const Statement & s) {
    actions.insert (actions.begin (), s.clone ());
  }

  /// pretty print
  void print (std::ostream & os, int ind) const {
    for (const_iterator it = begin() ; it != end() ; ++it) {
      (*it)->print(os, ind);
      os << ";" << std::endl;
    }
  }
  
  /// simplifies the current expression by asserting the value of a variable.
 // Assignment operator&(const Assertion &) const;
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const {
    std::set<Variable> toret;
    for (const_iterator it = begin() ; it != end() ; ++it) {
      std::set<Variable> tmp = (*it)->getSupport();
      toret.insert(tmp.begin(), tmp.end());
    }
    return toret;
  }

  /// equality comparison
  bool operator==(const Statement &s) const {
    return actions == ( (const Sequence &) s ).actions ;
  }
  /// ordering (for use with sets and maps)
  bool operator< (const Statement &s) const {
    return actions < ( (const Sequence &) s ).actions ;
  }

  StatementType getType() const { return SEQ; }
  
  Statement * clone () const { return new Sequence(*this); }

  void acceptVisitor (class StatementVisitor & vis) const { vis.visitSequence(*this) ; }    
};

/** 
 * An if (condition) { actionTrue } else { action false } statement
 * */ 
class Ite : public Statement {
private :
  BoolExpression cond;
  Sequence ifTrue;
  Sequence ifFalse;
public :
  /// Constructor, by reference since IntExpr are by construction unique
  Ite () {}
  Ite (const BoolExpression & ccond, const Sequence & iifTrue, const Sequence & iifFalse=Sequence() ) : cond(ccond.eval ()), ifTrue(iifTrue), ifFalse(iifFalse) {}

  const BoolExpression & getCondition() const { return cond; }
  Sequence & getIfTrue() { return ifTrue; }
  Sequence & getIfFalse() { return ifFalse; }
  const Sequence & getIfTrue() const { return ifTrue; }
  const Sequence & getIfFalse() const { return ifFalse; }
  void setCondition (const BoolExpression & e) { cond = e.eval (); }
 
  /// pretty print
  void print (std::ostream & os, int ind) const {
    indent(os,ind);
    os << "if (";
    cond.print(os);
    os << ") {" << std::endl;
    ifTrue.print(os, ind+1);
    
    indent(os,ind);
    os << "}" ;
    if (ifFalse.size() != 0) {
      os << " else {";
      ifFalse.print(os, ind +1);
      indent(os,ind);
      os << "}" << std::endl;
    } else {
      os << std::endl;
    }
  }
  
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const {
    std::set<Variable> toret;
    {
      std::set<Variable> tmp = cond.getSupport();
      toret.insert(tmp.begin(), tmp.end());

      tmp = ifTrue.getSupport();
      toret.insert(tmp.begin(), tmp.end());

      tmp = ifFalse.getSupport();
      toret.insert(tmp.begin(), tmp.end());
    }
    return toret;
  }

  /// equality comparison
  bool operator==(const Statement &s) const {
    return cond == ( (const Ite  &) s ).cond ;
  }
  /// ordering (for use with sets and maps)
  bool operator< (const Statement &s) const {
    return cond < ( (const Ite &) s ).cond ;
  }

  StatementType getType() const { return IfThenElse; }
  
  Statement * clone () const { return new Ite(*this); }

  void acceptVisitor (class StatementVisitor & vis) const { vis.visitIte(*this) ; }    
};

/** 
 * A While(condition) { action }  statement
 * */ 
class FixStatement : public Statement {
private :
  Sequence action;
public :
  /// Constructor, by reference since IntExpr are by construction unique
  FixStatement(const Sequence & s=Sequence()) : action(s) {}

  Sequence & getAction() { return action; }
  const Sequence & getAction() const { return action; }
 
  /// pretty print
  void print (std::ostream & os, int ind) const {
    indent(os,ind);
    os << "fixpoint {" << std::endl;
    action.print(os, ind+1);
    indent(os,ind);
    os << "}" ;
    os << std::endl;
  }
  
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const {
    return action.getSupport();
  }

  /// equality comparison
  bool operator==(const Statement &s) const {
    return action == ( (const FixStatement  &) s ).action ;
  }
  /// ordering (for use with sets and maps)
  bool operator< (const Statement &s) const {
    return action < ( (const FixStatement &) s ).action ;
  }

  StatementType getType() const { return FIXPOINT; }
  
  Statement * clone () const { return new FixStatement(*this); }

  void acceptVisitor (class StatementVisitor & vis) const { vis.visitFix(*this) ; }    
};



/** 
 * A call statement
 * */ 
class Call : public Statement {
private :
  vLabel label;
public :
  /// Constructor, by reference since IntExpr are by construction unique
  Call (Label lab) : label(lab) {}

  Label getLabel() const { return label; }
 
  /// pretty print
  void print (std::ostream & os, int ind) const {
    indent(os,ind);
    os << "self.\"" << label << "\"";
  }
  
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const {
    // DO NOT FOLLOW CALL TARGET(S) : support is empty
    return std::set<Variable> ();
  }

  /// equality comparison
  bool operator==(const Statement &s) const {
    return label == ( (const Call  &) s ).label ;
  }
  /// ordering (for use with sets and maps)
  bool operator< (const Statement &s) const {
    return label < ( (const Call &) s ).label ;
  }

  StatementType getType() const { return CALL; }
  
  Statement * clone () const { return new Call(*this); }

  void acceptVisitor (class StatementVisitor & vis) const { vis.visitCall(*this) ; }    
};


/** 
 * An abort kills the exploration : it returns an empty set of successors.
 * */ 
class AbortStatement : public Statement {
public :
  /// Constructor, by reference since IntExpr are by construction unique
  AbortStatement () {}
  

  /// pretty print
  void print (std::ostream & os, int ind) const {
    indent(os,ind);
    os << "abort ;" << std::endl;
  }
  
  /// simplifies the current expression by asserting the value of a variable.
 // Assignment operator&(const Assertion &) const;
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const {
    return std::set<Variable>();
  }

  /// equality comparison
  bool operator==(const Statement &s) const {
    return true;
  }
  /// ordering (for use with sets and maps)
  bool operator< (const Statement &s) const {
    return false;
  }

  StatementType getType() const { return ABORT; }
  
  Statement * clone () const { return new AbortStatement(); }

  void acceptVisitor (class StatementVisitor & vis) const { vis.visitAbort() ; }    
};






} // ns its


#endif
