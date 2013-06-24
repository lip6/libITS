#include "GALVarOrderHeuristic.hh"

#include "force.hh"
#include "PIntExprVisitor.hh"

namespace its {

/********** lexicographical heuristic *************/

static int parseFirstInt (Label s) {
  int val = -1 ;
  const char * sc = s.c_str();
  for (const char  *cp = sc ; *cp ; ++ cp ) {
    if (*cp >= '0' && *cp <= '9'){
      sscanf (cp, "%d" , &val);
    }
  }
  return val;
}

static bool less_var ( Label a , Label b ) {
  int vala = parseFirstInt(a) ;
  int valb = parseFirstInt(b) ;
  // Case 1 : global vars (no ints in them) are heavy, they go to top of structure.
  if ( vala == -1 && valb >= 0 ) {
    return false;
  } else if ( valb == -1 && vala >= 0 ) {
    return true;
  }
  // case 2 : we have two indexed variables, same index, revert to lexico
  else if ( vala == valb ) {
    return a < b;
  }
  // case 3 : sort by value
  else {
    return vala < valb;
  }
}

labels_t
lex_heuristic (const GAL * const g)
{
  labels_t vnames ;
  for (GAL::arrays_it it = g->arrays_begin() ; it != g->arrays_end() ; ++it ) {
    for (ArrayDeclaration::vars_it jt = it->vars_begin() ; jt != it->vars_end() ; ++ jt ) {
      vnames.push_back(jt->getName());
    }
  }
  for (GAL::vars_it it = g->vars_begin() ; it != g->vars_end(); ++it ) {
    vnames.push_back(it->getName());
  }
  
  // sort attempt to get variables closer together.
  sort(vnames.begin(), vnames.end(), & less_var ) ;
  
  return vnames;
}

/********** force heuristic ****************/

// a type to associate a variable to an int
typedef std::map<std::string,int> vtoi_t;

// an expression visitor to retrieve the variables
class GetVariableVisitor : public PIntExprVisitor, public PBoolExprVisitor {
  env_t env;
  std::set< std::string > res;
public:
  GetVariableVisitor(const env_t & l): env(l) {}
  
  // base case
  void visitVarExpr (int v) { res.insert (IntExpressionFactory::getVar (env[v])); }
  void visitArrayConstExpr (int i, const PIntExpression & e)
  {
    std::stringstream tmp;
    tmp << IntExpressionFactory::getVar (env[i]) << "[" << e.getValue () << "]";
    res.insert (tmp.str ());
  }
  
  // nothing to do
  void visitConstExpr (int) {}
  void visitBoolConstExpr (bool) {}
  
  // recursive call
  void visitWrapBoolExpr (const PBoolExpression & e) { e.accept (this); }
  void visitArrayVarExpr (int i, const PIntExpression & e) { e.accept (this); }
  void visitNaryIntExpr (IntExprType, const NaryPParamType & n)
  {
    for (NaryPParamType::const_iterator it = n.begin ();
         it != n.end (); ++it)
    {
      it->accept (this);
    }
  }
  void visitBinaryIntExpr (IntExprType, const PIntExpression & l, const PIntExpression & r)
  {
    l.accept (this);
    r.accept (this);
  }
  void visitNaryBoolExpr (BoolExprType, const std::vector<PBoolExpression> & n)
  {
    for (std::vector<PBoolExpression>::const_iterator it = n.begin ();
         it != n.end (); ++it)
    {
      it->accept (this);
    }
  }
  void visitBinaryBoolComp (BoolExprType, const PIntExpression & l, const PIntExpression & r)
  {
    l.accept (this);
    r.accept (this);
  }
  void visitNotBoolExpr (const PBoolExpression & e) { e.accept (this); }
  
  // accessor
  const std::set< std::string > & getResult () const { return res; }
};

class LocalityEdge : public constraint_t {
public:
  LocalityEdge (const std::set<var_t> & v): constraint_t (v) {}
  
  float
  cost (const order_t & o) const
  {
    // the cost of such a constraint is its span
    // that is the distance between the min index and the max index
    float max = -1, min = o.size ();
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      float i = o.find (*it)->second;
      if (i < min)
      {
        min = i;
      }
      if (i > max)
      {
        max = i;
      }
    }
    return max - min;
  }
  
  float
  cog (const order_t & o) const
  {
    float res = 0;
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      res += o.find (*it)->second;
    }
    res /= (float)data_.size ();
    return res;
  }
};

namespace {

class LocalConstraintBuilder : public StatementVisitor {
  const GAL * const gal_;
  std::vector<const constraint_t *> & constraints_;
  orderHeuristicType strat_;
  std::set<std::string> res;
  bool call_flag_;
public :
  // constructor
  LocalConstraintBuilder (const its::GAL * const g,
                          std::vector<const constraint_t *> & c,
                          orderHeuristicType s)
  : gal_(g)
  , constraints_(c)
  , strat_(s)
  , res(std::set<std::string>())
  , call_flag_(false)
  {}
  // destructor
  ~LocalConstraintBuilder () {} 
  
  /**** visit statements ****/
  void visitAssign (const class Assignment & ai) {
    GetVariableVisitor v1 (ai.getVariable ().getEnv ());
    ai.getVariable ().getExpr ().accept (&v1);
    GetVariableVisitor v2 (ai.getExpression ().getEnv ());
    ai.getExpression ().getExpr ().accept (&v2);
    
    res.insert (v1.getResult ().begin (), v1.getResult ().end ());
    res.insert (v2.getResult ().begin (), v2.getResult ().end ());
  } 
  
  void visitSequence (const class Sequence & seq) {
    for (Sequence::const_iterator it = seq.begin() ; it != seq.end() ; ++ it) {
      (*it)->acceptVisitor(*this);
    }
  } 

  void visitIte (const class Ite & ite) {
    handleBoolExpr(ite.getCondition());      
    ite.getIfTrue().acceptVisitor(*this);
    ite.getIfFalse().acceptVisitor(*this);
  }

  void visitFix (const class FixStatement & loop) {
    loop.getAction().acceptVisitor(*this);
  }

  void visitCall (const class Call & call) {
    // currently, only the default strat does not follow the call
    if (strat_ == DEFAULT)
      return;

    // set the call flag
    call_flag_ = true;
    // add a constraint that gathers all the variables of the callees
    for (GAL::trans_it it = gal_->trans_begin ();
         it != gal_->trans_end (); ++it)
    {
      if (it->getLabel () == call.getLabel ())
      {
        this->handleBoolExpr (it->getGuard ());
        it->getAction ().acceptVisitor (*this);
      }
    }
  }
  
  void visitAbort () {
    // nothing to do
  } 

  void handleBoolExpr (const BoolExpression & be) {
    GetVariableVisitor v (be.getEnv ());
    be.getExpr().accept (&v);
    res.insert (v.getResult ().begin (), v.getResult ().end ());
  }

  // process a guardedAction
  void
  process (const GuardedAction & g, const vtoi_t & var_to_int)
  {
    handleBoolExpr (g.getGuard ());
    g.getAction ().acceptVisitor (*this);
    
    // add the constraint only if it has more than one variable
    if (res.size () > 1)
    {
      std::set<var_t> tmp;
      for (std::set<std::string>::const_iterator it = res.begin ();
           it != res.end (); ++it)
      {
        vtoi_t::const_iterator vi = var_to_int.find (*it);
        assert (vi != var_to_int.end ());
        tmp.insert (vi->second);
      }
      LocalityEdge * e = new LocalityEdge (tmp);
      // \todo what weight for a transition featuring a call ?
//      if (call_flag_)
//        e->setWeight (2);
      constraints_.push_back (e);
    }
  }
};

void
add_locality_constraint (std::vector<const constraint_t *> & c,
                         const GAL * gal,
                         orderHeuristicType s,
                         const GuardedAction & g,
                         const vtoi_t & var_to_int)
{
  LocalConstraintBuilder lcb (gal, c, s);
  lcb.process (g, var_to_int);
}

} // anonymous namespace

labels_t
force_heuristic (const GAL * const g, orderHeuristicType strat)
{
  // compute the lexicographical ordering on variables
  // this is used to map variables to integers (for FORCE, variables are int).
  // it will also be used as the initial ordering for FORCE
  labels_t vnames = lex_heuristic (g);
  vtoi_t var_to_int;
  for (size_t i = 0; i != vnames.size (); ++i)
  {
    var_to_int [vnames [i]] = i;
  }
  
  std::vector<const constraint_t *> constraints;
  // walk the transitions of the GAL
  for (GAL::trans_it it = g->trans_begin ();
       it != g->trans_end (); ++it)
  {
    // add locality constraint
    add_locality_constraint (constraints, g, strat, *it, var_to_int);
  }
  
  // build the initial ordering from the lexicographical ordering
  order_t init_order;
  int i = 0;
  for (labels_t::const_iterator it = vnames.begin ();
       it != vnames.end (); ++it)
  {
    init_order [var_to_int [*it]] = i++;
  }
  
  // call the FORCE algorithm
  order_t new_order = force (constraints, init_order);
  
  // rebuild a labels_t according to the order given by FORCE
  labels_t result (new_order.size ());
  for (order_t::const_iterator it = new_order.begin ();
       it != new_order.end (); ++it)
  {
    result [it->second] = vnames [it->first];
  }
  
  // delete the constraints
  for (std::vector<const constraint_t *>::const_iterator it = constraints.begin ();
       it != constraints.end (); ++it)
  {
    delete *it;
  }
  
  return result;
}

} // namespace its
