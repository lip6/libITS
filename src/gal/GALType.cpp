#include "GALType.hh"

#include "Observe_Hom.hh"
#include "Hom_Basic.hh"
#include "ExprHom.hpp"

#include "divine/dve2GAL.hh"

#include <algorithm>

#include "force.hh"
#include "PIntExprVisitor.hh"

#define DEFAULT_VAR 0

namespace its {

/** the set T of public transition labels (a copy)*/
labels_t GALType::getTransLabels () const {
  labels_t ret;

  for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
    ret.push_back(it->getName());
  }
  return ret;
}

  /** To obtain a representation of a labeled state */
  State GALType::getState(Label) const {
    // converting to DDD first
    DDD M0 = DDD::one;
    const VarOrder & vo = *getVarOrder();
    // each place = one var as indicated by getPorder
    for (size_t i=0 ; i < vo.size() ; ++i) {
      Label pname = vo.getLabel(i);
      // retrieve the appropriate place marking
      int mark = gal_->getVarValue(pname);
      // left concatenate to M0
      M0 = DDD (i,mark) ^ M0;
      // for pretty print
      DDD::varName(i,pname);
    }
    return State(DEFAULT_VAR, M0);
  }

  GHom GALType::buildHom(const GuardedAction & it) const {

    GHom guard = predicate ( it.getGuard(), getVarOrder());
    GHom action = GHom::id;
    for (GuardedAction::actions_it jt = it.begin() ; jt != it.end() ; ++ jt) {
      GHom todo;
      if (jt->getVariable().getType() == VAR && jt->getExpression().getType() == CONST)
	todo = setVarConst ( getVarOrder()->getIndex(jt->getVariable().getName()), jt->getExpression().getValue());
      else
	todo =  assignExpr(jt->getVariable(), jt->getExpression(),getVarOrder());
      action = todo & action;
    }
    return action & guard;
  }

    /** state and transitions representation functions */
  /** Local transitions */
  Transition GALType::getLocals () const {

    d3::set<GHom>::type toadd;
    NaryBoolParamType stutter_trans;
    for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
      if (it->getLabel() == "")
      {
        toadd.insert(buildHom(*it) );
        stutter_trans.insert(! it->getGuard ());
      }
    }
    if (stutterOnDeadlock)
      toadd.insert (predicate (BoolExpressionFactory::createNary (AND, stutter_trans), getVarOrder ()));
    
    GHom sum = GHom::add(toadd);

    // so "sum" contains the successor relationship
    // now factor in the transient states
    GHom next = fixpoint(ITE ( predicate(gal_->isTransientState(), getVarOrder()), sum, GHom::id)) & sum;

    return localApply(  next, DEFAULT_VAR );
  }

  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  Transition GALType::getSuccs (const labels_t & tau) const {
    Hom resultTrans = Hom::id;

    // iterate on labels
    for (labels_t::const_iterator it = tau.begin() ; it != tau.end() ; ++it) {
      Hom labelAction;
      bool isFirstTrWithLabel = true;
      for (GAL::trans_it t = gal_->trans_begin() ; t != gal_->trans_end() ; ++t) {
	if (t->getLabel() == *it) {
	  if (! isFirstTrWithLabel) {
	    labelAction = labelAction +  buildHom(*t) ;
	  } else {
	    // add the effect of this action
	    labelAction =  buildHom(*t) ;
	    isFirstTrWithLabel = false;
	  }
	}
      }
      if (isFirstTrWithLabel) {
	std::cerr << "Asked for succ by transition "<< *it << " but no such transition label exists in type " << gal_->getName() << std::endl ;
	assert(false);
      }
      resultTrans = labelAction & resultTrans ;
    }
    // The DDD state variant uses an intermediate variable called DEFAULT_VAR
    return localApply(  resultTrans, DEFAULT_VAR );
  }

  /** Return the set of local transitions, with their name, useful for diplaying.*
   * Used in witness trace/counter example construction scenarios.
   **/
  void GALType::getNamedLocals (namedTrs_t & ntrans) const {
    for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
      if (it->getLabel() == "" )
	ntrans.push_back(std::make_pair(it->getName() , localApply(buildHom(*it),DEFAULT_VAR)) );
    }
  }



  /** To obtain the potential state space of a Type : i.e. the cartesian product of variable domains.
   *  Uses the provided "reachable" states to compute the variable domains. */
  State GALType::getPotentialStates(State reachable) const {
    // Note : code copy pasted from TPNetSemantics, refactor to share ?

    const VarOrder & vo = * getVarOrder();
    // converting to DDD first
    DDD M0 = DDD::one;
    const DDD * reach = (const DDD * ) reachable.begin()->first ;
    // each place = one var as indicated by getPorder
    for (size_t i=0 ; i < vo.size() ; ++i) {
      // retrieve the appropriate place marking
      DDD dom = computeDomain (i,*reach);
      // left concatenate to M0
      M0 = dom ^ M0;
    }
    return State( DEFAULT_VAR , DDD(M0));
  }


  /** Return a Transition that maps states to their observation class.
   *  Observation class is based on the provided set of observed variables,
   *  in standard "." separated qualified variable names.
   *  The returned Transition replaces the values of non-observed variables
   *  by their domain.
   **/
  Transition GALType::observe (labels_t obs, State potential) const {
    if (obs.empty()) {
      return potential;
    }

    const VarOrder & vo = *getVarOrder();

    its::vars_t obs_index;
    obs_index.reserve(obs.size());
    // each place = one var as indicated by varOrder
    for (int i=vo.size()-1 ; i >= 0  ; --i) {
      Label varname = vo.getLabel(i);

      labels_it it = find(obs.begin(), obs.end(),varname);
      if (it != obs.end()) {
	// var is observed
	obs_index.push_back(i);
      }
    }

    return localApply ( observeVars(obs_index,* ( (const DDD *) potential.begin()->first) ), DEFAULT_VAR );
  }


  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition GALType::getAPredicate (Label predicate) const {


    AtomicPredicate pred = parseAtomicPredicate(predicate);


    //      std::cerr << "Petri net parsed predicate var:" << var << " comp:" << comp << " value:"<<val <<std::endl;
    //      std::cerr << "Translates to hom :" << Semantics::getHom ( foo, index, value) << std::endl;

    return  localApply( (*pred.comp) (pred.var , pred.val), DEFAULT_VAR );
  }

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

  static
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

  class ArrayVisitor : public PIntExprVisitor {
    int res;
  public:
    ArrayVisitor(): res(-1) {}
    
    // nothing to do
    void visitVarExpr (int) {}
    // nothing to do
    void visitConstExpr (int) {}
    // nothing to do
    void visitWrapBoolExpr (const class PBoolExpression &) {}
    // get Array name
    void visitArrayVarExpr (int i, const class PIntExpression &) { res = i; }
    // get Array name
    void visitArrayConstExpr (int i, const class PIntExpression &) { res = i; }
    // nothing to do
    void visitNaryIntExpr (IntExprType, const NaryPParamType &) {}
    // nothing to do
    void visitBinaryIntExpr (IntExprType, const class PIntExpression &, const class PIntExpression &) {}
    
    int getResult () const { return res; }
  };
  
  static
  labels_t
  force_heuristic (const GAL * const g)
  {
    std::set< edge > constraint;
    std::set< std::string > non_const_array;
    // walk the transitions of the GAL
    for (GAL::trans_it it = g->trans_begin ();
         it != g->trans_end (); ++it)
    {
      // walk the actions of the transition
      for (GuardedAction::actions_it ai = it->begin ();
           ai != it->end (); ++ai)
      {
        IntExpression var = ai->getVariable ();
        std::string lhs;
        
        // if var is a variable
        if (var.getType () == VAR)
        {
          lhs = var.getName ();
        }
        // else, var is an array access
        else
        {
          // if var is a const array access, it can be seen as a single variable
          if (var.getType () == CONSTARRAY)
          {
            lhs = ai->getVariable ().getName ();
          }
          // else, the array should remain contiguous
          else
          {
            ArrayVisitor av;
            ai->getVariable ().getExpr ().accept (&av);
            lhs = var.getEnv () [av.getResult ()];
            // remember that this array has a non-const access
            non_const_array.insert (lhs);
            // \todo add the index constraints
          }
        }
        
        // fill the constraints
        labels_t env = ai->getExpression ().getEnv ();
        for (labels_t::const_iterator ei = env.begin (); ei != env.end (); ++ei)
        {
          // \todo check the const and non-const array accesses
          if (ei->find_first_of ('[') == std::string::npos)
          {
            constraint.insert (std::make_pair (lhs, *ei));
          }
        }
      }
    }
    
    // if we have found a non-const array 'tab',
    // remove the constraints with a const access to 'tab'
    {
      std::set< edge > c_tmp;
      for (std::set< edge >::const_iterator it = constraint.begin ();
           it != constraint.end (); ++it)
      {
        std::string e1 = it->first.substr (0, it->first.find_first_of ('['));
        std::string e2 = it->second.substr (0, it->second.find_first_of ('['));
        // avoid the self constraint (x < x): stupid case
        if (    (it->first != it->second)
            &&  (non_const_array.find (e1) == non_const_array.end ())
            &&  (non_const_array.find (e2) == non_const_array.end ()))
        {
          c_tmp.insert (*it);
          std::cerr << "constraint added " << it->first << " , " << it->second << std::endl;
        }
      }
      constraint = c_tmp;
    }
    
    // pathological case: if no constraints have been found (ex: phils)
    // the use the lexicographical heuristic
    if (constraint.empty ())
    {
      return lex_heuristic (g);
    }
    // build the initial order
    // first get all the variables
    // the tab names for those that have non-const accesses
    // and tab variables for the others
    labels_t vnames ;
    for (GAL::arrays_it it = g->arrays_begin() ; it != g->arrays_end() ; ++it )
    {
      if ( non_const_array.find (it->getName()) != non_const_array.end () )
      {
        vnames.push_back (it->getName ());
      }
      else
      {
        for (ArrayDeclaration::vars_it jt = it->vars_begin ();
             jt != it->vars_end (); ++jt)
        {
          vnames.push_back (jt->getName ());
        }
      }
    }
    for (GAL::vars_it it = g->vars_begin() ; it != g->vars_end(); ++it )
    {
      vnames.push_back (it->getName ());
    }
    
    // build the initial ordering
    // arbitrarily from vnames
    // \todo randomize this initial ordering ??
    order init_order;
    int i = 0;
    for (labels_t::const_iterator it = vnames.begin ();
         it != vnames.end (); ++it)
    {
      init_order [*it] = i++;
    }
    
    // call the force algorithm
    order n_order = force (constraint, init_order);
    
    // build a labels_t according to the order given by force
    labels_t tmp = labels_t (n_order.size ());
    for (order::const_iterator it = n_order.begin ();
         it != n_order.end (); ++it)
    {
      tmp[it->second] = it->first;
    }
    labels_t result;
    for (labels_t::const_iterator it = tmp.begin ();
         it != tmp.end (); ++it)
    {
      bool is_array = false;
      for (GAL::arrays_it ai = g->arrays_begin() ; ai != g->arrays_end() ; ++ai )
      {
        if (*it == ai->getName ())
        {
          is_array = true;
          for (ArrayDeclaration::vars_it jt = ai->vars_begin() ; jt != ai->vars_end() ; ++ jt )
          {
            result.push_back (jt->getName ());
          }
          break;
        }
      }
      if (! is_array)
      {
        result.push_back (*it);
      }
    }
    
    return result;
  }
  
  labels_t GALType::getVarSet () const
  {
    //labels_t res = force_heuristic (gal_);
    labels_t res = lex_heuristic (gal_);
    
    for (labels_t::const_iterator it = res.begin ();
         it != res.end (); ++it)
    {
      std::cerr << (*it) << ",";
    }
    std::cerr << std::endl;
    
    return res;
  }
  
  /********* class GALDVEType ************/

  GALDVEType::GALDVEType (const GAL * g, dve2GAL::dve2GAL * d): GALType (g)
  {
    setDVE2GAL (d);
  }

  GALDVEType::GALDVEType (const GAL * g): GALType (g) {}

  void GALDVEType::setDVE2GAL (dve2GAL::dve2GAL * d)
  {
    dve = d;
  }

  Transition GALDVEType::getAPredicate (Label predicate) const
  {
    std::string new_pred;
    // if the given predicate is of the form "P.CS"
    // turn it into "P.state=0" (if 0 is the index of CS
    // this happens only if the predicate does have (=|<|>|<=|>=|!=) in it
    if (    (predicate.find_first_of ('=') == std::string::npos)
	&&  (predicate.find_first_of ('<') == std::string::npos)
	&&  (predicate.find_first_of ('>') == std::string::npos))
    {
      // look for the '.' that separates between process' name and state's name
      size_t dot_pos = predicate.find_first_of ('.');
      assert (dot_pos != std::string::npos);
      // get process name
      std::string proc = predicate.substr (0, dot_pos);
      // get state name
      std::string state = predicate.substr (dot_pos+1, predicate.size()-dot_pos-1);
      int nb_state = -1;
      // look for the index of the state name in process
      for (size_t i = 0 ; i < dve->get_process_count () && nb_state == -1; ++i)
      {
	if (! proc.compare (dve->get_symbol_table ()->get_process (i)->get_name ()))
	{
	  divine::dve_process_t * current_process = dynamic_cast<divine::dve_process_t*> (dve->get_process (i));
	  assert (current_process);
	  for (size_t j = 0 ; j < current_process->get_state_count () ; ++j)
	  {
	    size_t sgid = current_process->get_state_gid (j);
	    if (! state.compare (current_process->get_symbol_table ()->get_state (sgid)->get_name ()))
	    {

	      // This piece of code track question on a single state
	      // inside of a Process
	      if ( current_process->get_state_count () == 1)
		{
		  return Transition::id;
		}

	      nb_state = j;
	      break;
	    }
	  }
	}
      }
      assert (nb_state >= 0);
      // build the new string
      std::stringstream tmp;
      tmp << proc << ".state=" << nb_state;
      new_pred = tmp.str ();
    }
    else
    {
      // nothing to do
      new_pred = predicate;
    }

    return GALType::getAPredicate (new_pred);
  }

  /******** class GALDVETypeFactory **************/

  GALType * GALTypeFactory::createGALType (const GAL * g)
  {
    return new GALType (g);
  }

  GALType * GALTypeFactory::createGALDVEType (Label path, bool stutterOnDeadlock)
  {
    struct dve2GAL::dve2GAL * loader = new dve2GAL::dve2GAL ();
    std::string modelName = wibble::str::basename(path);
    loader->setModelName(modelName);
    loader->read( path.c_str() );
    loader->analyse();
    GALType * res = new GALDVEType (loader->convertFromDve(), loader);
    res->setStutterOnDeadlock (stutterOnDeadlock);
    return res;
  }

} // namespace
