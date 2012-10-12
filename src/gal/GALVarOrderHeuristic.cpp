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

class GetArrayVisitor : public PIntExprVisitor, public PBoolExprVisitor {
  std::vector< std::pair<int,PIntExpression> > res;
public:
  GetArrayVisitor () {}
  
  // nothing to do
  void visitVarExpr (int) {}
  void visitConstExpr (int) {}
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
  void visitWrapBoolExpr (const class PBoolExpression &) {}
  void visitArrayConstExpr (int, const class PIntExpression &) {}
  void visitNaryIntExpr (IntExprType, const NaryPParamType &) {}
  void visitBinaryIntExpr (IntExprType, const class PIntExpression &, const class PIntExpression &) {}

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

class GetVariableVisitor : public PIntExprVisitor, public PBoolExprVisitor {
  labels_t env;
  std::set< std::string > res;
public:
  GetVariableVisitor(const labels_t & l): env(l) {}
  
  // base case
  void visitVarExpr (int v) { res.insert (env[v]); }
  void visitArrayConstExpr (int i, const PIntExpression & e)
  {
    std::stringstream tmp;
    tmp << env[i] << "[" << e.getValue () << "]";
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

class QueryEdge : public edge_t {
public:
  QueryEdge (const std::string & l, const std::string & r)
  {
    data_.push_back (l);
    data_.push_back (r);
  }
  
  int
  cost (const order_t & o) const
  {
    int e1 = o.find (data_[0])->second;
    int e2 = o.find (data_[1])->second;
    return (e1 < e2) ? 0 : (e1-e2);
  }
  
  cog_t
  cog (const order_t & o) const
  {
    int e1 = o.find (data_[0])->second;
    int e2 = o.find (data_[1])->second;
    cog_t res;
    if (e1 < e2)
    {
      res[data_[0]] = e1;
      res[data_[1]] = e2;
    }
    else
    {
      res[data_[0]] = e2;
      res[data_[1]] = e1;
    }
    return res;
  }
};

class LocalityEdge : public edge_t {
public:
  LocalityEdge (const std::vector<std::string> & v): edge_t (v) {}
  
  int
  cost (const order_t & o) const
  {
    // the cost of such a constraint is its span
    // that is the distance between the min index and the max index
    int max = -1, min = o.size ();
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      int i = o.find (*it)->second;
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
    tmp /= data_.size ();
    cog_t res;
    for (const_iterator it = begin ();
         it != end (); ++it)
    {
      res[*it] = tmp;
    }
    return res;
  }
};

class StateEdge : public edge_t {
public:
  StateEdge (const std::string & c)
  {
    data_.push_back (c);
  }
  
  int
  cost (const order_t & o) const
  {
    int e = o.find (data_[0])->second;
    return o.size () - e;
  }
  
  cog_t
  cog (const order_t & o) const
  {
    cog_t res;
    res [data_ [0]] = o.size ();
    return res;
  }
};

namespace {

template<class Expr>
void
add_query_constraint (std::vector<const edge_t *> & c, const Expr & g, const GAL & gal)
{
  // query constraints
  GetArrayVisitor gav;
  g.getExpr ().accept (&gav);
  
  labels_t env = g.getEnv ();
  for (std::vector< std::pair<int, PIntExpression> >::const_iterator it = gav.getResult ().begin ();
       it != gav.getResult ().end (); ++it)
  {
    std::set< std::string > lhs, rhs;
    std::string current_array = env [it->first];
    // walk the GAL looking for the current_array
    for (GAL::arrays_it ai = gal.arrays_begin ();
         ai != gal.arrays_end (); ++ai)
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
          edge_t * tmp = new QueryEdge (*li, *ri);
          c.push_back (tmp);
        }
      }
    }
  }
}

void
add_locality_constraint (std::vector<const edge_t *> & c, const BoolExpression & g, const GAL & gal)
{
  // locality constraints
  std::set<std::string> local_edge;
  GetVariableVisitor v(g.getEnv ());
  g.getExpr ().accept (&v);
  std::vector<std::string> tmp2 (v.getResult ().begin(), v.getResult ().end ());
  if (tmp2.size () > 1)
    c.push_back (new LocalityEdge (tmp2));
}

void
add_locality_constraint (std::vector<const edge_t *> & c, const Assignment & g, const GAL & gal)
{
  // locality constraints
  GetVariableVisitor v1(g.getVariable ().getEnv ());
  g.getVariable ().getExpr ().accept (&v1);
  
  GetVariableVisitor v2(g.getExpression ().getEnv ());
  g.getExpression ().getExpr ().accept (&v2);
  
  std::set<std::string> local_edge = v1.getResult ();
  local_edge.insert (v2.getResult ().begin (), v2.getResult ().end ());
  
  std::vector<std::string> tmp2 (local_edge.begin(), local_edge.end ());
  if (tmp2.size () > 1)
    c.push_back (new LocalityEdge (tmp2));
}

} // anonymous namespace

labels_t
force_heuristic (const GAL * const g)
{
  // get the array names
  std::set< std::string > arrays;
  for (GAL::arrays_it it = g->arrays_begin() ; it != g->arrays_end() ; ++it)
  {
    arrays.insert (it->getName ());
  }
  
  // get the constraints
  std::vector< const edge_t * > constraint;
  // walk the transitions of the GAL
  for (GAL::trans_it it = g->trans_begin ();
       it != g->trans_end (); ++it)
  {
    // add the arrays constraints for the guard
    add_query_constraint (constraint, it->getGuard (), *g);
    add_locality_constraint (constraint, it->getGuard (), *g);
    
    // walk the actions of the transition
    for (GuardedAction::actions_it ai = it->begin ();
         ai != it->end (); ++ai)
    {
      add_query_constraint (constraint, ai->getVariable (), *g);
      add_query_constraint (constraint, ai->getExpression (), *g);
      add_locality_constraint (constraint, *ai, *g);
      
      IntExpression var = ai->getVariable ();
      std::set< std::string > lhs;
      
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
        ai->getVariable ().getExpr ().accept (&av);
        std::string current_array = var.getEnv () [av.getResult ()];
        // find the array in the GAL
        for (GAL::arrays_it ari = g->arrays_begin ();
             ari != g->arrays_end (); ++ari)
        {
          if (current_array == it->getName ())
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
      GetVariableVisitor vv (ai->getExpression ().getEnv ());
      ai->getExpression ().getExpr ().accept (&vv);
      std::set< std::string > rhs = vv.getResult ();
      
      for (std::set< std::string >::const_iterator li = lhs.begin ();
           li != lhs.end (); ++li)
      {
        for (std::set< std::string >::const_iterator ri = rhs.begin ();
             ri != rhs.end (); ++ri)
        {
          if (*li != *ri)
          {
            edge_t * tmp = new QueryEdge (*li, *ri);
            constraint.push_back (tmp);
          }
        }
      }
    }
  }
  
  // use the lexicographical heuristic to get initial order
  labels_t vnames = lex_heuristic (g);
  
  // build the initial ordering
  // arbitrarily from vnames
  // \todo randomize this initial ordering ??
  order_t init_order;
  int i = 0;
  for (labels_t::const_iterator it = vnames.begin ();
       it != vnames.end (); ++it)
  {
    // add state constraints
    size_t pos = it->find_last_of ('.');
    if (pos != std::string::npos)
    {
      std::string tail = it->substr (pos+1, it->size () - pos - 1);
      if (tail == "state")
      {
        constraint.push_back (new StateEdge (*it));
      }
    }
    
    init_order [*it] = i++;
  }
  
  // call the force algorithm
  order_t n_order = force (constraint, init_order);
  
  // build a labels_t according to the order given by force
  labels_t tmp = labels_t (n_order.size ());
  for (order_t::const_iterator it = n_order.begin ();
       it != n_order.end (); ++it)
  {
    tmp[it->second] = it->first;
  }
  labels_t result;
  for (labels_t::const_iterator it = tmp.begin ();
       it != tmp.end (); ++it)
  {
    result.push_back (*it);
  }
  
  // delete the contraints
  for (std::vector<const edge_t *>::const_iterator it = constraint.begin ();
       it != constraint.end (); ++it)
  {
    delete *it;
  }
  
  return result;
}

} // namespace its
