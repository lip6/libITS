#include "GALType.hh"

#include "Observe_Hom.hh"
#include "Hom_Basic.hh"
#include "ExprHom.hpp"

#include "divine/dve2GAL.hh"
#include "gal/parser/GALParser.hh"

#include <algorithm>

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


  class HomBuilder : public StatementVisitor {
  public :
    Hom res;
    const GALType & context;

    HomBuilder(const GALType & c): res(GHom::id), context(c) {}

    ~HomBuilder() {} 
    
    void visitAssign (const class Assignment & ass) {
      if (ass.getVariable().getType() == VAR && ass.getExpression().getType() == CONST) {
	res = setVarConst ( context.getVarOrder()->getIndex(ass.getVariable().getName()), ass.getExpression().getValue());
      } else if (ass.getVariable().getType() == CONSTARRAY && ass.getExpression().getType() == CONST) {
	res = setVarConst ( context.getVarOrder()->getIndex(ass.getVariable().getName()), ass.getExpression().getValue());
      }	else {
	res =  assignExpr( ass.getVariable(), ass.getExpression(), context.getGalOrder());
      }
    } 

    void visitIncrAssign (const class IncrAssignment & ass) {
      if (ass.getVariable().getType() == VAR && ass.getExpression().getType() == CONST) {
	res = incVar ( context.getVarOrder()->getIndex(ass.getVariable().getName()), ass.getExpression().getValue());
      } else if (ass.getVariable().getType() == CONSTARRAY && ass.getExpression().getType() == CONST) {
	res = incVar ( context.getVarOrder()->getIndex(ass.getVariable().getName()), ass.getExpression().getValue());
      }	else {
	res =  incrExpr( ass.getVariable(), ass.getExpression(), context.getGalOrder());
      }
    } 


    void visitSequence (const class Sequence & seq) {
      for (Sequence::const_iterator it = seq.begin() ; it != seq.end() ; ++ it) {
	res = context.buildHom(**it) & res ;
      }
    } 

    void visitIte (const class Ite & ite) {
      res = ITE( predicate(ite.getCondition(), context.getGalOrder()), context.buildHom(ite.getIfTrue()), context.buildHom(ite.getIfFalse()));
    }

    void visitFix (const class FixStatement & loop) {
      res = fixpoint (  context.buildHom(loop.getAction())) ;
    }

    void visitCall (const class Call & call) {
      labels_t tau = labels_t(1, call.getLabel());
      res = context.getSuccsHom(tau);
    } 

    void visitAbort () {
      res = res & GDDD::null;
    } 

  };

  GHom GALType::buildHom(const Statement & s) const {
    HomBuilder hb (*this);
    s.acceptVisitor(hb);
    return hb.res;
  }

  GHom GALType::buildHom(const GuardedAction & ga) const {
    GHom guard = predicate ( ga.getGuard().eval().pushNegations(), getGalOrder());
    GHom action = buildHom( ga.getAction());
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
      toadd.insert (predicate (BoolExpressionFactory::createNary (AND, stutter_trans).eval(), getGalOrder ()));
    
    GHom sum = GHom::add(toadd);

    // so "sum" contains the successor relationship
    // now factor in the transient states
    GHom next = fixpoint(ITE ( predicate(gal_->isTransientState().eval(), getGalOrder()), sum, GHom::id)) & sum;

    return localApply(  next, DEFAULT_VAR );
  }

  GHom GALType::getSuccsHom (const labels_t & tau) const {
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
    return resultTrans;
  }

  /** Successors synchronization function : Bag(T) -> SHom.
   * The collection represented by the iterator should be a multiset
   * of transition labels of this type (as obtained through getTransLabels()).
   * Otherwise, an assertion violation will be raised !!
   * */
  Transition GALType::getSuccs (const labels_t & tau) const {
    GHom resultTrans = getSuccsHom(tau);
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
  
  labels_t GALType::getVarSet () const
  {
    labels_t res;
    res = force_heuristic (gal_, strat_);
    
    // Debug
    // for (labels_t::const_iterator it = res.begin ();
    //      it != res.end (); ++it)
    // {
    //   std::cerr << (*it) << ",";
    // }
    // std::cerr << std::endl;
    
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
  
  vLabel
  GALDVEType::predicate_dve2gal (Label predicate) const
  {
    std::stringstream new_pred;
    
    size_t i = 0;
    while (i != predicate.size ())
    {
      if (predicate[i] == '.')
      {
        // get the name of the process
        int p_begin = i-1;
        while (p_begin >= 0 && (isalnum (predicate[p_begin]) || predicate[p_begin] == '_'))
          --p_begin;
        std::string process = predicate.substr (p_begin+1, i-p_begin-1);
        // get the name of the state
        size_t s_end = i+1;
        while (s_end < predicate.size () && (isalnum (predicate[s_end]) || predicate[s_end] == '_'))
          ++s_end;
        std::string state = predicate.substr (i+1, s_end-i-1);
        
        // for this variable:
        // -1 means "uninitialized", i.e. the state index has not been found yet
        // -2 means that the corresponding process has a single state, so "P.state" is to be replaced by "true"
        // any other value is positive and corresponds to the state index
        int nb_state = -1;
        // look for the index of the state name in process
        for (size_t j = 0 ; j < dve->get_process_count () && nb_state == -1; ++j)
        {
          if (! process.compare (dve->get_symbol_table ()->get_process (j)->get_name ()))
          {
            divine::dve_process_t * current_process = dynamic_cast<divine::dve_process_t*> (dve->get_process (j));
            assert (current_process);
            for (size_t k = 0 ; k < current_process->get_state_count () ; ++k)
            {
              size_t sgid = current_process->get_state_gid (k);
              if (! state.compare (current_process->get_symbol_table ()->get_state (sgid)->get_name ()))
              {
                // This piece of code track question on a single state
                // inside of a Process
                if ( current_process->get_state_count () == 1)
                  nb_state = -2;
                else
                  nb_state = k;
                // done, early cut of the loop
                break;
              }
            }
          }
        }
        
        // if no state has been found, then it was not a state variable, do nothing
        if (nb_state == -1)
        {
          new_pred << ".";
          ++i;
          continue;
        }
        assert (nb_state < -2 || nb_state >= 0);
        
        // get the string in the bugger
        std::string new_pred_tmp = new_pred.str ();
        // remove the process name
        new_pred_tmp = new_pred_tmp.substr (0, new_pred_tmp.size () - process.size ());
        // reset the stream with the new string
        new_pred.str (new_pred_tmp);
        // reset the put pointer
        new_pred.seekp (new_pred_tmp.size ());
        // open '('
        new_pred << "(";
        
        // if the process has a single state
        if (nb_state == -2)
          new_pred << "True";
        else
          new_pred << process << ".state == " << nb_state;
        
        // close ')'
        new_pred << ")";
        
        // increment position
        i = s_end;
      }
      else
      {
        new_pred << predicate[i];
        ++i;
      }
    }
    return new_pred.str ();
  }
  
  BoolExpression
  GALDVEType::getBPredicate (Label pred) const
  {
    vLabel new_pred = predicate_dve2gal (pred);
    return GALType::getBPredicate (new_pred);
  }

  /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition GALType::getPredicate (char * pred) const {
    // std::cerr << "pred : " << pred << std::endl;
    // std::cerr << its::predicate (getBPredicate (pred), getGalOrder ()) << std::endl;
    return localApply (its::predicate ( getBPredicate (pred).eval().pushNegations(), getGalOrder ()), DEFAULT_VAR);
  }
  
  BoolExpression GALType::getBPredicate (Label pred) const {
    return GALParser::parsePredicate (pred, gal_).eval();
  }

} // namespace

// GAL parser
// moved includes here because on MingW some includes derived from antlr dependencies # define as a macro the string CONST
// This messes with CONST, the enum value in a PIntExpression
//#include "gal/parser/exprParserLexer.h"
//#include "gal/parser/exprParserParser.h"
//#include <antlr3.h>
