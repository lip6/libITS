#ifndef ___GAL__H__
#define ___GAL__H__

#include <string>
#include <utility>
#include <iosfwd>
#include <map>

// hash_map from libddd, for initial state
#include "util/configuration.hh"

#include "BoolExpression.hh"
#include "Variable.hh"
#include "IntExpression.hh"


namespace its {

class Assignment {
  // Note : the lhs should be of type Variable or ArrayAccess, ultimately resolved to a variable.
  // It is an error if the lhs resolves to a constant value.
  IntExpression var_;
  // A contrario, the rhs should evaluate to a constant
  IntExpression expr_;
    
  public :
  Assignment (const IntExpression & var, const IntExpression & expr) : var_(var), expr_(expr) {}
  const IntExpression & getVariable () const { return var_; }
  const IntExpression & getExpression () const { return expr_; }
  void print (std::ostream & os) const;
};

class GuardedAction : public NamedElement {
public :
  typedef std::vector<Assignment> actions_t;
  typedef actions_t::const_iterator actions_it;
private :
  BoolExpression guard_;
  actions_t actions_;
public:
  GuardedAction (Label name):NamedElement(name),guard_(BoolExpressionFactory::createConstant(true)) {};
  void setGuard (const BoolExpression & guard) { guard_ = guard.eval(); }
  const BoolExpression & getGuard () const { return guard_; }
  actions_it begin() const { return actions_.begin() ; }
  actions_it end() const { return actions_.end() ; }
  void addAction (const Assignment & ass) { actions_.push_back(ass);}
  void print (std::ostream & os) const;
};

class GALState {
public :
  typedef std::map<vLabel,int> state_t;
  typedef state_t::const_iterator const_iterator;
private :
  state_t state_;

public :
  void setVarValue (Label var, int val) {
    state_.insert(std::make_pair(var,val));
  }

  int getVarValue (Label var) const {
    const_iterator access = state_.find(var);
    if (access != state_.end()) return access->second;     
    return -1;
  }

  const_iterator begin() const { return state_.begin() ; }
  const_iterator end() const { return state_.end() ; }

};

/** A class to represent a Guarded Action Language model. *
 */
  class GAL : public NamedElement {
public :
  typedef std::vector<Variable> vars_t;
  typedef vars_t::const_iterator vars_it;

  typedef std::vector<GuardedAction> trans_t;
  typedef trans_t::const_iterator trans_it;

  typedef GALState state_t;
  typedef state_t::const_iterator state_it;
private :
  vars_t variables_;
  trans_t transitions_;
  state_t init_;
  BoolExpression transient_;
public :
    GAL (Label name) : NamedElement(name), transient_(BoolExpressionFactory::createConstant(false)) {};
  
  void addVariable(const Variable & var, int value) { 
    variables_.push_back(var); 
    init_.setVarValue(var.getName(),value);
  }
  void addTransition (const GuardedAction & act) { transitions_.push_back(act); }  

  vars_it vars_begin() const { return variables_.begin() ; }
  vars_it vars_end() const { return variables_.end() ; }
  trans_it trans_begin() const { return transitions_.begin() ; }
  trans_it trans_end() const { return transitions_.end() ; }
  state_it state_begin() const { return init_.begin() ; }
  state_it state_end() const { return init_.end() ; }
  int getVarValue (Label name) const {
    return init_.getVarValue(name);
  }

  const BoolExpression & isTransientState () const {
    return transient_;
  }

  void setTransientPredicate (const BoolExpression & expr) {
    transient_ = expr.eval();
  }

};


  std::ostream & operator<< ( std::ostream &, const GAL &); 

}


#endif // include protection
