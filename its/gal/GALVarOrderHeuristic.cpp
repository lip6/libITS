#include "GALVarOrderHeuristic.hh"

#include "its/gal/force.hh"
#include "its/gal/PIntExprVisitor.hh"
#include "its/gal/GALType.hh"

namespace its {

/********** lexicographical heuristic *************/

/** Actually parses LAST integer currently */
static int parseFirstInt (Label s) {
  int val = -1 ;
  const char * sc = s.c_str();
  for (const char  *cp = sc ; *cp ; ++ cp ) {
    if (*cp >= '0' && *cp <= '9'){
      sscanf (cp, "%d" , &val);
      int copy = val;
      while (copy /= 10) {
	++cp;
      }
    }
  }
  return val;
}

bool less_var ( Label a , Label b ) {
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
  void visitBoolNDefExpr () {}
  void visitNDefExpr () {}
  
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
  void visitUnaryIntExpr (IntExprType, const PIntExpression & l)
  {
    l.accept (this);
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

class QueryEdge : public constraint_t {
  var_t l, r;
public:
  QueryEdge (var_t ll, var_t rr)
  : constraint_t (), l (ll), r (rr)
  {
    data_.insert (l);
    data_.insert (r);
  }
  
  float
  cost (const order_t & o) const
  {
    int p1 = o.find (l)->second;
    int p2 = o.find (r)->second;
    return (p1 < p2) ? 0 : (p1-p2);
  }
  
  cog_t
  cog (const order_t & o) const
  {
    int p1 = o.find (l)->second;
    int p2 = o.find (r)->second;
    
    cog_t res;
    if (p1 < p2)
    {
      res [l] = p1;
      res [r] = p2;
    }
    else
    {
      res [l] = p2;
      res [r] = p1;
    }
    return res;
  }
};

class LocalityEdge : public constraint_t {
  orderHeuristicType strat_;
public:
  LocalityEdge (const std::set<var_t> & v, orderHeuristicType s)
  : constraint_t (v)
  , strat_ (s)
  {}
  
  float
  cost (const order_t & o) const
  {
    // those strategies are based on a squared cost
    // physical approach, we compute the energy of the springs
    // the energy of a spring is quadratic in its elongation
    if (strat_ == FOLLOW_SQ ||
        strat_ == FOLLOW_DYN_SQ ||
        strat_ == FOLLOW_FDYN_SQ)
    {
      // all the variables are supposed to have the same cog, take the first one
      float c = cog (o).begin ()->second;
      float energy = 0;
      for (const_iterator it = begin ();
           it != end (); ++it)
      {
        float l = ((float)o.find (*it)->second) - c;
        energy += l*l;
      }
      return energy;
    }
    
    // for all other strategies
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
  
  cog_t
  cog (const order_t & o) const
  {
    float tmp = 0;
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      tmp += o.find (*it)->second;
    }
    tmp /= (float)data_.size ();
    
    cog_t res;
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      res [*it] = tmp;
    }
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

  void visitSyncAssign (const class SyncAssignment & sa) {
    for (auto const& ai : sa.getAssignments ())
    {
      GetVariableVisitor v1 (ai.first.getEnv ());
      ai.first.getExpr ().accept (&v1);
      GetVariableVisitor v2 (ai.second.getEnv ());
      ai.second.getExpr ().accept (&v2);

      res.insert (v1.getResult ().begin (), v1.getResult ().end ());
      res.insert (v2.getResult ().begin (), v2.getResult ().end ());
    }
  }

  void visitIncrAssign (const class IncrAssignment & ai) {
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
        if (vi == var_to_int.end ()) {
	  std::cerr << "Could not access index of variable :" << *it;
	  assert (vi != var_to_int.end ());
	}
        tmp.insert (vi->second);
      }
      LocalityEdge * e = new LocalityEdge (tmp, strat_);
      // \todo what weight for a transition featuring a call ?
      float esize = e->size ();
      float tsize = var_to_int.size ();
      
      switch (strat_)
      {
        case DEFAULT:
          if (! call_flag_)
            constraints_.push_back (e);
          else
            delete e;
          break;
        case FOLLOW:
        case FOLLOW_SQ:
        case SATUR:
          constraints_.push_back (e);
          break;
        case FOLLOW_HALF:
          if (call_flag_)
            e->setWeight (0.5);
          constraints_.push_back (e);
          break;
        case FOLLOW_DOUBLE:
          if (call_flag_)
            e->setWeight (2);
          constraints_.push_back (e);
          break;
        case FOLLOW_DYN:
        case FOLLOW_DYN_SQ:
          if (call_flag_)
            e->setWeight (esize / tsize);
          constraints_.push_back (e);
          break;
        case FOLLOW_FDYN:
        case FOLLOW_FDYN_SQ:
          e->setWeight (esize / tsize);
          constraints_.push_back (e);
          break;
        case LEXICO: // should not happen
          assert (false);
          break;
      }
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

class GetArrayVisitor : public PIntExprVisitor, public PBoolExprVisitor {
  std::vector< std::pair<int,PIntExpression> > res;
public:
  GetArrayVisitor () {}
  
  // nothing to do
  void visitVarExpr (int) {}
  void visitConstExpr (int) {}
  void visitNDefExpr () {}
  void visitBoolNDefExpr () {}
  void visitArrayConstExpr (int, const PIntExpression &) {}
  void visitBoolConstExpr (bool) {}
  
  // recursive calls
  void visitWrapBoolExpr (const PBoolExpression & e) { e.accept (this); }
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
  void visitUnaryIntExpr (IntExprType, const PIntExpression & l)
  {
    l.accept (this);
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
  
  // base case
  void visitArrayVarExpr (int a, const PIntExpression & i)
  {
    std::vector<std::pair<int,PIntExpression> >::const_iterator it;
    for (it = res.begin ();
         it != res.end (); ++it)
    {
      if (it->first == a && it->second.equals (i))
      {
        break;
      }
    }
    // if the pair is not already in the vector
    if (it == res.end ())
    {
      res.push_back (std::make_pair (a, i));
    }
    i.accept (this);
  }
  
  // accessor
  const std::vector< std::pair<int,PIntExpression> > & getResult () const { return res; }
};

class ArrayVisitor : public PIntExprVisitor {
  int res;
  PIntExpression index;
public:
  ArrayVisitor(): res(-1), index(PIntExpressionFactory::createConstant (0)) {}
  
  // nothing to do
  void visitVarExpr (int) {}
  void visitConstExpr (int) {}
  void visitNDefExpr () {}
  void visitWrapBoolExpr (const class PBoolExpression &) {}
  void visitArrayConstExpr (int, const class PIntExpression &) {}
  void visitNaryIntExpr (IntExprType, const NaryPParamType &) {}
  void visitBinaryIntExpr (IntExprType, const class PIntExpression &, const class PIntExpression &) {}
  void visitUnaryIntExpr (IntExprType, const class PIntExpression &) {}
  
  // get Array name
  void visitArrayVarExpr (int i, const PIntExpression & e)
  {
    res = i;
    index = e;
  }
  
  // accessors
  int getResult () const { return res; }
  PIntExpression getIndex () const { return index; }
};

template<class Expr>
void
add_query_constraint (std::vector<const constraint_t *> & c,
                      const Expr & g,
                      const GAL * gal,
                      const vtoi_t & var_to_int)
{
  // query constraints
  GetArrayVisitor gav;
  g.getExpr ().accept (&gav);
  
  env_t env = g.getEnv ();
  for (std::vector< std::pair<int, PIntExpression> >::const_iterator it = gav.getResult ().begin ();
       it != gav.getResult ().end (); ++it)
  {
    std::set< std::string > lhs, rhs;
    std::string current_array = IntExpressionFactory::getVar (env [it->first]);
    // walk the GAL looking for the current_array
    for (GAL::arrays_it ai = gal->arrays_begin ();
         ai != gal->arrays_end (); ++ai)
    {
      if (ai->getName () == current_array)
      {
        for (ArrayDeclaration::vars_it vi = ai->vars_begin ();
             vi != ai->vars_end (); ++vi)
        {
          lhs.insert (vi->getName ());
        }
        break;
      }
    }
    
    GetVariableVisitor vv (env);
    it->second.accept (&vv);
    rhs = vv.getResult ();
    // query constraints
    for (std::set<std::string>::const_iterator li = lhs.begin ();
         li != lhs.end (); ++li)
    {
      for (std::set<std::string>::const_iterator ri = rhs.begin ();
           ri != rhs.end (); ++ri)
      {
        std::string e1 = li->substr (0, li->find_first_of ('['));
        std::string e2 = ri->substr (0, ri->find_first_of ('['));
        if (e1 != e2)
        {
          constraint_t * tmp = new QueryEdge (var_to_int.find (*li)->second,
                                              var_to_int.find (*ri)->second);
          c.push_back (tmp);
        }
      }
    }
  }
}
  
class QueryConstraintBuilder : public StatementVisitor {
  const vtoi_t & vtoi;
public:
  std::vector<const constraint_t *> & c;
  const GAL * gal;
  
  /// constructor
  QueryConstraintBuilder (std::vector<const constraint_t *> & cc,
                          const GAL * gal,
                          const vtoi_t & v)
  : vtoi (v), c (cc), gal (gal) {}
  /// destructor
  ~QueryConstraintBuilder () {} 
  
  void
  visitAssign (const class Assignment & ai)
  {
    visitAnyAssign(ai.getVariable(), ai.getExpression());
  }
  void
  visitIncrAssign (const class IncrAssignment & ai)
  {
    visitAnyAssign(ai.getVariable(), ai.getExpression());
  }

  void 
  visitAnyAssign (const IntExpression & var, const IntExpression & value) 
  {
    // add the array constraints for the lhs
    add_query_constraint (c, var, gal, vtoi);
    // add the array constraints for the rhs
    add_query_constraint (c, value, gal, vtoi);
    
    // add the assignment constraints
    std::set<std::string> lhs;
    // if var is a variable
    if (var.getType () == VAR || var.getType () == CONSTARRAY)
    {
      lhs.insert (var.getName ());
    }
    // else, var is a non-const array access
    else
    {
      // find the array in the GAL
      ArrayVisitor av;
      var.getExpr ().accept (&av);
      std::string current_array = IntExpressionFactory::getVar (var.getEnv () [av.getResult ()]);
      // find the array in the GAL
      for (GAL::arrays_it ari = gal->arrays_begin ();
           ari != gal->arrays_end (); ++ari)
      {
        if (current_array == ari->getName ())
        {
          // add the variables of the array to lhs
          for (ArrayDeclaration::vars_it vi = ari->vars_begin ();
               vi != ari->vars_end (); ++vi)
          {
            lhs.insert (vi->getName ());
          }
          // leave the loop
          break;
        }
      }
    }
    
    // fill the constraints
    GetVariableVisitor vv (value.getEnv ());
    value.getExpr ().accept (&vv);
    std::set< std::string > rhs = vv.getResult ();
    
    for (std::set< std::string >::const_iterator li = lhs.begin ();
         li != lhs.end (); ++li)
    {
      for (std::set< std::string >::const_iterator ri = rhs.begin ();
           ri != rhs.end (); ++ri)
      {
        if (*li != *ri)
        {
          constraint_t * tmp = new QueryEdge (vtoi.find (*li)->second,
                                              vtoi.find (*ri)->second);
          c.push_back (tmp);
        }
      }
    }
    
  }

  void
  visitSyncAssign (const class SyncAssignment & ass)
  {
    // act as if a sequence of assignments
    for (auto const & ai : ass.getAssignments ())
    {
      visitAnyAssign (ai.first, ai.second);
    }
  }
  
  void
  visitSequence (const class Sequence & seq)
  {
    for (Sequence::const_iterator it = seq.begin ();
         it != seq.end (); ++ it)
    {
      (*it)->acceptVisitor (*this);
    }
  } 
  
  void
  visitIte (const class Ite & ite)
  {
    add_query_constraint (c, ite.getCondition (), gal, vtoi);      
    ite.getIfTrue ().acceptVisitor (*this);
    ite.getIfFalse ().acceptVisitor (*this);
  }
  
  void
  visitFix (const class FixStatement & loop)
  {
    loop.getAction ().acceptVisitor (*this);
  }
  
  void
  visitCall (const class Call & call)
  {
    // Calls are not followed for query constraints => no additional constraints
  }
  
  void
  visitAbort ()
  {
    // nothing to do
  }
};

void
add_query_constraint (std::vector<const constraint_t *> & c,
                      const GAL * gal,
                      const GuardedAction & g,
                      const vtoi_t & var_to_int)
{
  // handle the guard
  add_query_constraint (c, g.getGuard (), gal, var_to_int);
  
  QueryConstraintBuilder qcb (c, gal, var_to_int);
  g.getAction ().acceptVisitor (qcb);
}

} // anonymous namespace

labels_t
force_heuristic (const GAL * const g, orderHeuristicType strat)
{
  // compute the lexicographical ordering on variables
  // this is used to map variables to integers (for FORCE, variables are int).
  // it will also be used as the initial ordering for FORCE
  labels_t vnames = lex_heuristic (g);
  
  if (strat == LEXICO)
    return vnames;
  
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
    // add query constraint
    add_query_constraint (constraints, g, *it, var_to_int);
  }
  
  // build the initial ordering from the lexicographical ordering
  order_t init_order;
  int i = 0;
  for (labels_t::const_iterator it = vnames.begin ();
       it != vnames.end (); ++it)
  {
    init_order [var_to_int [*it]] = i++;
  }
  
  if (strat == SATUR)
  {
    // count the number of constraints that have the same set of variables
    std::map<std::set<var_t>, int> count;
    for (std::vector<const constraint_t *>::const_iterator it = constraints.begin ();
         it != constraints.end (); ++it)
    {
      count [(*it)->get_data ()] += 1;
    }
    for (std::vector<const constraint_t *>::const_iterator it = constraints.begin ();
         it != constraints.end (); ++it)
    {
//      std::cerr << count[(*it)->get_data ()] << std::endl;
      const_cast<constraint_t *> (*it)->set_dev (count[(*it)->get_data ()]);
    }
  }
  
  // \debug
  std::map<int, std::string> int_to_var;
  for (std::map<std::string,int>::const_iterator it = var_to_int.begin ();
       it != var_to_int.end (); ++it)
  {
    int_to_var[it->second] = it->first;
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
