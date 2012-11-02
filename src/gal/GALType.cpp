#include "GALType.hh"

#include "Observe_Hom.hh"
#include "Hom_Basic.hh"
#include "ExprHom.hpp"

#include "divine/dve2GAL.hh"

#include <algorithm>

#include "GALVarOrderHeuristic.hh"

#define DEFAULT_VAR 0

namespace its {

/** the set T of public transition labels (a copy)*/
labels_t GALType::getTransLabels () const {
  labels_t ret;

  for (GAL::trans_it it = gal_->trans_begin() ; it != gal_->trans_end() ; ++it) {
    ret.push_back(it->getLabel());
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

    GHom guard = predicate ( it.getGuard(), getGalOrder());
    GHom action = GHom::id;
    for (GuardedAction::actions_it jt = it.begin() ; jt != it.end() ; ++ jt) {
      GHom todo;
      if (jt->getVariable().getType() == VAR && jt->getExpression().getType() == CONST)
	todo = setVarConst ( getVarOrder()->getIndex(jt->getVariable().getName()), jt->getExpression().getValue());
      else
	todo =  assignExpr(jt->getVariable(), jt->getExpression(),getGalOrder());
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
      toadd.insert (predicate (BoolExpressionFactory::createNary (AND, stutter_trans), getGalOrder ()));
    
    GHom sum = GHom::add(toadd);

    // so "sum" contains the successor relationship
    // now factor in the transient states
    GHom next = fixpoint(ITE ( predicate(gal_->isTransientState(), getGalOrder()), sum, GHom::id)) & sum;

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
  
  labels_t GALType::getVarSet () const
  {
    labels_t res;
    res = force_heuristic (gal_, voLocal, voQuery, voState);
    
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

  bool
  GALDVEType::setVarOrder (labels_t v) const
  {
    // first remove the state variables for the processes with only one state
    labels_t new_v;
    for (labels_t::const_iterator it = v.begin ();
         it != v.end (); ++it)
    {
      // if the order is produced by ETF, the state variable of process P is named P
      // we want to remove such states variables for processes with only one state
      
      // whether the current label is a state variable
      bool is_state_var = false;
      // whether we keep the current label
      bool keepit = true;
      
      // walk the processes of the DVE model
      for (size_t i = 0 ; i < dve->get_process_count () ; ++i)
      {
        // if the current variable is the name of a process
        // then it is a state variable
        if (! it->compare (dve->get_symbol_table ()->get_process (i)->get_name ()))
        {
          is_state_var = true;
          divine::dve_process_t * current_process = dynamic_cast<divine::dve_process_t*> (dve->get_process (i));
          if (current_process->get_state_count () == 1)
          {
            keepit = false;
          }
          break;
        }
      }
      
      if (keepit)
      {
        if (is_state_var)
        {
          std::stringstream tmp;
          tmp << *it << ".state";
          new_v.push_back (tmp.str ());
        }
        else
        {
          new_v.push_back (*it);
        }
      }
    }
    
    return GALType::setVarOrder (new_v);
  }
  
  Transition GALDVEType::getAPredicate (Label predicate) const
  {
    std::string new_pred;
    // if the given predicate is of the form "P.CS"
    // turn it into "P.state=0" (if 0 is the index of CS
    // this happens only if the predicate does not have (=|<|>|<=|>=|!=) in it
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

  /******** class GALTypeFactory **************/

  void
  GALTypeFactory::parseHeuristicOptions (const std::string & v, GALType * gt)
  {
    assert (v.size () < 4);
    for (size_t i = 0 ; i < v.size () ; ++i)
    {
      switch (v[i])
      {
        case 'L':
          gt->setVoLocal (true);
          break;
        case 'Q':
          gt->setVoQuery (true);
          break;
        case 'S':
          gt->setVoState (true);
          break;
        default:
          // not valid
          std::cerr << "invalid VarOrder heuristic option " << v[i] << std::endl;
          assert(false);
      }
    }
  }
  
  GALType * GALTypeFactory::createGALType (const GAL * g, bool stutterOnDeadlock, const std::string & varOrderHeuristic)
  {
    GALType * res = new GALType (g);
    res->setStutterOnDeadlock (stutterOnDeadlock);
    parseHeuristicOptions (varOrderHeuristic, res);
    return res;
  }

  GALType * GALTypeFactory::createGALDVEType (Label path, bool stutterOnDeadlock, const std::string & varOrderHeuristic)
  {
    struct dve2GAL::dve2GAL * loader = new dve2GAL::dve2GAL ();
    std::string modelName = wibble::str::basename(path);
    loader->setModelName(modelName);
    loader->read( path.c_str() );
    loader->analyse();
    GALType * res = new GALDVEType (loader->convertFromDve(), loader);
    res->setStutterOnDeadlock (stutterOnDeadlock);
    
    parseHeuristicOptions (varOrderHeuristic, res);
    return res;
  }

} // namespace
