#include "mc/ctlCheck.hh"
#include "ctlp/visfd.h"

#ifdef ITS_EXERCISE
#include "operators.hh"
#endif

using namespace its;


its::Transition  CTLChecker::getHomomorphism (Ctlp_Formula_t *ctlFormula) const {
  its::Transition stop = Shom(GSDD::top);
  its::Transition result= stop;

  if (Ctlp_FormulaReadType(ctlFormula) == Ctlp_NOT_c) {
    Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
    if (Ctlp_FormulaReadType(leftChild) == Ctlp_NOT_c) {
      return getHomomorphism(Ctlp_FormulaReadLeftChild(leftChild));
    }
  }


  ctl_cache_it it = ctl_cache.find(ctlFormula);
  if ( it == ctl_cache.end() ) {
    // A miss 
    // invoke recursive procedures
    its::Transition leftHom, rightHom;

    // Handle left child
    {
      Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
      if (leftChild) {
	leftHom = getHomomorphism (leftChild);
	if (leftHom == stop) {
	  return stop;
	}
      }
    }
    // Handle right child
    {
      Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);
      if (rightChild) {
	rightHom = getHomomorphism (rightChild);
	if (rightHom == stop) {
	  return stop;
	}
      }      
    }
    //    std::cerr << "Translating CTL formula : " ;
    //    Ctlp_FormulaPrint(vis_stderr, ctlFormula);
    //    std::cerr << std::endl;

    // Now switch on the type of formula
    switch (Ctlp_FormulaReadType(ctlFormula)) {
    case Ctlp_ID_c:
      // basic case
      result =  getSelectorAP( Ctlp_FormulaReadVariableName(ctlFormula),Ctlp_FormulaReadValueName(ctlFormula) );
      break;
    case Ctlp_TRUE_c:
      result = Transition::id;
      break;

    case Ctlp_FALSE_c:
      result = Transition::null; 
      break;

    case Ctlp_NOT_c:
      result = ! leftHom  ; 
      break;
      
    case Ctlp_AND_c:
      result =  leftHom & rightHom ;
      break;

    case Ctlp_EQ_c:
      assert(false);
//      result = mdd_xnor(leftMdd, rightMdd); 
      break;

    case Ctlp_XOR_c:
      assert(false);
//      result = mdd_xor(leftMdd, rightMdd); break;
      break;

    case Ctlp_THEN_c:
      // A => B    <->  not A or B
      result = (! leftHom ) + rightHom ;
      break;

    case Ctlp_OR_c:
      result = leftHom + rightHom ;
      //      result = leftHom + rightHom;
      break;

    case Ctlp_EX_c:
      //      result = ( ( getPredRel()& leftHom & getNextRel() ) * Transition::id );
      result = stop;
      break;

    case Ctlp_EU_c: 
      // start from states satisfying g, then add predescessors verifying f in a fixpoint.
      // f U g <-> ( f & pred + Id )^* & g
      //      result = fixpoint ( (leftHom & getPredRel()) + its::Transition::id ) & rightHom ;
      result = stop;
      break;
    
    case Ctlp_EG_c: 
      // start with states satisfying f
      // then remove states that are not a predescessor of a state in the set
      // EG f <->  ( f & pred )^* & f
      result = stop;
      //      result = fixpoint ( leftHom & getPredRel() * its::Transition::id ) & leftHom ;
      break;
    
    case Ctlp_Cmp_c: {
      result = stop;
      // Forward CTL specific operator, curretly unimplemented
      //      assert(false);
//       if (Ctlp_FormulaReadCompareValue(ctlFormula) == 0)
// 	result = bdd_is_tautology(leftMdd, 0) ?
// 	  mdd_one(mddMgr) : mdd_zero(mddMgr);
//       else
// 	result = bdd_is_tautology(leftMdd, 0) ?
// 	  mdd_zero(mddMgr) : mdd_one(mddMgr);
      break;
    }
    case Ctlp_Init_c:
      // cast to constant homomorphism
      result = stop; // result = GShom(getInitialState()) ;
      break;
    case Ctlp_FwdU_c:
      /** From Vis source documentation :
       *							    t
       ** E[p U q]      = lfp Z[q V (p ^ EX(Z))]   :   p p ... p q
       ** FwdUntil(p,q) = lfp Z[p V EY(Z ^ q)]     :		    pq q q ... q
       */
      /**
       * In other words, start from states satisfying p, then add successors satisfying q to fixpoint
       * FwdUntil(p,q) =  ( q & Next  + Id)^* & p
       */
      // test for trivial reachability case
      // if (Ctlp_FormulaReadLeftChild(ctlFormula) &&
      // 	  Ctlp_FormulaReadType(Ctlp_FormulaReadLeftChild(ctlFormula)) ==
      // 	  Ctlp_Init_c &&
      // 	  Ctlp_FormulaReadRightChild(ctlFormula) &&
      // 	  Ctlp_FormulaReadType(Ctlp_FormulaReadRightChild(ctlFormula)) ==
      // 	  Ctlp_TRUE_c ) {
      // 	// cast to constant hom
      // 	result = GShom(getReachable()) ;
      // 	break;
      // }
      // // the real case
      // result = fixpoint ( (rightHom & getNextRel()) + its::Transition::id ) & leftHom ;
      result = stop;
      break;
    case Ctlp_FwdG_c:
      // states reachable by an infinite path of f
      //      result = fixpoint ( leftHom & getNextRel() ) ; 
      result = stop;
      break;
    case Ctlp_EY_c:
      // exists yesterday : states that have a predescessor that verifies f
      // take states verifying f, then compute successors
      // EY f  <->  Next & f (reach)
      //      result = getNextRel() & leftHom ;
      result = stop;
      break;
      
    default: 
      fail("Encountered unknown type of CTL formula\n");
    }

    // std::cerr << "Obtained Homomorphism : " << result << std::endl;
    
  } else {
    result = it->second;
  }  
  return result;
}


// Explain the truth value of formula in given states.
// Formula is true if at least some input states satify it.
// Writes to out a diagnosis.
// Returns states that were actually explained, a subset of the input states.
its::State CTLChecker::explain (its::State sat, Ctlp_Formula_t *ctlFormula, std::ostream & out) const  {
 
  out << "Subformula :";
  Ctlp_FormulaPrint(vis_stdout, ctlFormula);
  // test if formula is true in given states. i.e. some at least satisfy p.
  its::State trueF = getStateVerifying(ctlFormula);
  // Testing with subsets would be stronger, requiring all states satisfy p.
  its::State satF = sat * trueF ; 
  bool formIsTrue = ( satF != State::null ) ;
  if (trueF != State::one) {
    if (formIsTrue) {
      out << " is true in ";
      if (sat.nbStates()==1) {
	out << "the single input state :\n" ;
	model.printSomeStates(sat,out);
      } else {
	out << satF.nbStates() <<  " state(s) out of "<< sat.nbStates() << " input state(s).\n";
      }
    } else {
      out << " is false in all " << sat.nbStates() << " input state(s).\n"  ;
    }
  } // End of Forward CTL State::one case.
  // Handle left child
  Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
  // Handle right child
  Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);

  
  // Now switch on the type of formula
  switch (Ctlp_FormulaReadType(ctlFormula)) {
  case Ctlp_ID_c:
      {
	if (formIsTrue) {
	  if (satF.nbStates() > 1) {
	    out << "For instance predicate is true in these states :\n";
	    model.printSomeStates(satF, out);
	  }
	  return satF;
	} else {
	  out << "Because atomic predicate " << Ctlp_FormulaReadVariableName(ctlFormula) << Ctlp_FormulaReadValueName(ctlFormula) << " is false in all " << sat.nbStates()  << " input states. \n" <<  std::endl ;
	  out << "For instance predicate is false in this state :\n";
	  model.printSomeStates(sat, out);
	  return sat;
	}
	break;
      }
  case Ctlp_TRUE_c:
    out << "Because TRUE holds true in any state.\n";
    return sat;
    break;
    
  case Ctlp_FALSE_c:
    out << "Because FALSE is false in any state.\n";
    return sat;
    break;
    
  case Ctlp_NOT_c:
    if (formIsTrue) {
      out << "Because for some input states all future behaviors satisfy NOT(" ;
      Ctlp_FormulaPrint(vis_stdout, leftChild);
      out << ")" <<  std::endl;
      return explain(satF, leftChild, out);
    } else {
      out << "Because for some input states, there exist some future behaviors that satisfy " ;
      Ctlp_FormulaPrint(vis_stdout, leftChild);
      out <<  std::endl;
      return explain(sat, leftChild, out);
    }
    break;
    
  case Ctlp_AND_c:
    if (formIsTrue) {
      out << "Because both " ;
      Ctlp_FormulaPrint(vis_stdout,leftChild);
      out  << " and " ;
      Ctlp_FormulaPrint(vis_stdout,rightChild);
      out << " are satisfied in some input states. For instance :\n" ;
      its::State lexpl = explain(satF, leftChild, out); 
      out << "And in the same states  \n";
      its::State rexpl = explain(lexpl, rightChild, out); 
      return rexpl;
    } else {
      
      out << "Because no input states satisfy both " ;
      Ctlp_FormulaPrint(vis_stdout,leftChild) ;
      out << " and " ;
      Ctlp_FormulaPrint(vis_stdout,rightChild);
      out<< " simultaneously.\n" ;
      if (leftChild && rightChild) {
	its::State leftStates = getStateVerifying (leftChild) * sat ;
	its::State rightStates = getStateVerifying (rightChild) * sat ;
	
	// cut into cases, since a & b is false in s 
	// either : s*a = 0  or s*b = 0 (explain why) and its no use talking further
	// or both are non-empty but a&b false, so that (s*a) * (s*b) is empty.
	// explain both a and b and conclude in empty intersection.
	if (leftStates == its::State::null && rightStates == its::State::null) {
	  out << "Even formula " ;
	  Ctlp_FormulaPrint(vis_stdout,leftChild);
	  out << " OR " ;
	  Ctlp_FormulaPrint(vis_stdout,rightChild) ;
	  out << " is false in all input states.\n" ;
	  out << " For instance " ;
	  Ctlp_FormulaPrint(vis_stdout,leftChild);
	  out << " is not satisfied in input states.\n";
	  return explain(sat,leftChild,out);
	} else if (leftStates == its::State::null) {
	  out << "First conjunct  " ;
	  Ctlp_FormulaPrint(vis_stdout,leftChild) ;
	  out << " is not satisfied in any input states. So AND conjunction is false.\n";
	  return explain(sat,leftChild,out);
	} else if (rightStates == its::State::null) {
	  out << "Second conjunct  " ;
	  Ctlp_FormulaPrint(vis_stdout,rightChild) ;
	  out << " is not satisfied in any input states. So AND conjunction is false.\n";
	  return explain(sat,rightChild,out);
	} else {
	  out << "Some " << leftStates.nbStates() << " of your " << sat.nbStates() << " input states do satisfy " ;
	  Ctlp_FormulaPrint(vis_stdout,leftChild);
	  out << " AND some " << rightStates.nbStates() << " of your " << sat.nbStates() << " input states do satisfy " ;
	  Ctlp_FormulaPrint(vis_stdout,rightChild);
	  out << ".\nBut these sets (explained below) have an empty intersection.\n";
	  explain(sat, leftChild, out);
	  explain(sat, rightChild, out);
	  return sat;
	}
      } else {
	std::cerr << "ERROR : Conjunction has no children" << std::endl; 
	assert(false);
      }	
    }
    break;
    
  case Ctlp_EQ_c:
    assert(false);
    //      result = mdd_xnor(leftMdd, rightMdd); 
    break;
    
  case Ctlp_XOR_c:
    assert(false);
    //      result = mdd_xor(leftMdd, rightMdd); break;
    break;

  case Ctlp_THEN_c:
    // A => B    <->  not A or B
    if (formIsTrue) {
      if (leftChild && rightChild) {
	its::State leftStates = getStateVerifying (leftChild) * sat ;
	its::State rightStates = getStateVerifying (rightChild) * sat ;
	if (leftStates != its::State::null) {
	  out << "Because NOT " ;
	  Ctlp_FormulaPrint(vis_stdout,leftChild);
	  out << " is true in " << leftStates.nbStates() << " states of your " << sat.nbStates() << " input states." << std::endl;
	  return explain(leftStates, leftChild, out);
	} else {
	  out << "Because " ;
	  Ctlp_FormulaPrint(vis_stdout,rightChild);
	  out << " is true in " << rightStates.nbStates() << " states of your " << sat.nbStates() << " input states." << std::endl;
	  return explain(rightStates, leftChild, out);
	}
      } else {
	std::cerr << "ERROR : Implication has no children" << std::endl; 
	assert(false);
      }
    } else {
      out << "Because A=>B <==> !A OR B and  both NOT(" ;
      Ctlp_FormulaPrint(vis_stdout,leftChild);
      out  << ") and " ;
      Ctlp_FormulaPrint(vis_stdout,rightChild);
      out << " are unsatisfied in all input states. For instance A is true :\n" ;
      its::State lexpl = explain(sat, leftChild, out); 
      out << "And also (!B) is true \n";
      its::State rexpl = explain(sat, rightChild, out); 
      return sat;
    } 
    break;

  case Ctlp_OR_c:
    if (formIsTrue) {
      if (leftChild && rightChild) {
	its::State leftStates = getStateVerifying (leftChild) * sat ;
	its::State rightStates = getStateVerifying (rightChild) * sat ;
	if (leftStates != its::State::null) {
	  out << "Because " ;
	  Ctlp_FormulaPrint(vis_stdout,leftChild);
	  out << " is true in " << leftStates.nbStates() << " states of your " << sat.nbStates() << " input states." << std::endl;
	  return explain(leftStates, leftChild, out);
	} else {
	  out << "Because " ;
	  Ctlp_FormulaPrint(vis_stdout,rightChild);
	  out << " is true in " << rightStates.nbStates() << " states of your " << sat.nbStates() << " input states." << std::endl;
	  return explain(rightStates, leftChild, out);
	}
      } else {
	std::cerr << "ERROR : Implication has no children" << std::endl; 
	assert(false);
      }
    } else {
      out << "Because both A and B are unsatisfied in all input states. Both NOT(" ;
      Ctlp_FormulaPrint(vis_stdout,leftChild);
      out  << ") and NOT(" ;
      Ctlp_FormulaPrint(vis_stdout,rightChild);
      out << ") are true in all input states. For instance A is false :\n" ;
      its::State lexpl = explain(sat, leftChild, out); 
      out << "And also B is false \n";
      its::State rexpl = explain(sat, rightChild, out); 
      return sat;
    } 
    break;

  case Ctlp_EX_c:
    if (formIsTrue) {
      if (leftChild) {
	its::State leftStates = getStateVerifying (leftChild) ;
	// due to hypothesis on init and final being empty inter in findPath, have to do this manually
	//	its::path_t path = model.findPath(satF,getNextRel()(satF) * leftStates, getReachable(), true);
	vLabel tr;
	State target;
	Type::namedTrs_t trs;
	model.getNamedLocals(trs);
	for (const auto & it : trs ) {
	  target = it.second (satF) * leftStates;
	  if (target != State::null) {
	    tr = it.first;
	    satF = getPredRel() (target) * satF;
	    break;
	  }
	}
	// EX => should be exactly one transition
	assert(tr != "");
	labels_t cont;
	cont.push_back(tr);
	path_t path (cont, satF, target);
	out << "EX p is true Because there are immediate successors of input states that satisfy p.\n";
	out << "Following path leads from initial states to states satisfying p.\n";
	model.printPath(path,out,true);
	explain (path.getFinal(),leftChild,out);
	return path.getInit();
      } else {
	std::cerr << "ERROR : EX has no children" << std::endl; 
	assert(false);
      }
    } else {
      out << "EX p is false because all successors of current states satisfy !p.\n" ;
      out << "For instance some input states such as \n" ;
      model.printSomeStates(sat,out);
      out << " have successors such as  \n" ;
      explain ( getNextRel() (sat), leftChild, out);
      return sat;
    } 
    break;
  case Ctlp_EU_c: 
    {
      // start from states satisfying g, then add predescessors verifying f in a fixpoint.
      // f U g <-> ( f & pred + Id )^* & g
      //      result = fixpoint ( (leftStates * getPredRel()) + its::Transition::id )  (rightStates) ;
      if (formIsTrue) {
	if (leftChild && rightChild) {
	  its::State leftStates = getStateVerifying (leftChild) ;
	  its::State rightStates = getStateVerifying (rightChild) ;
	  its::State satB = satF * rightStates ;
	  if (satB != State::null) {
	    out << "E a U b is true because some input states immediately satisfy b. \n";
	    explain(satB,rightChild,out);
	  } else {

	    its::path_t path = model.findPath(satF,rightStates, leftStates, true);
	    out << "E a U b is true Because there are paths through states verifying a to states verifying b. \n";
	    out << "Such a minimal path of length " << path.getPath().size() << " is:\n" ;
	    model.printPath(path,out,true);
	    out << "Justification follows for subformulas.\n";
	    its::State toret = explain(path.getInit(),leftChild,out);
	    explain(path.getFinal(),rightChild,out);
	    return toret;
	  }
	} else {
	  std::cerr << "ERROR : EU has no children" << std::endl; 
	  assert(false);
	}
      } else {
	if (leftChild && rightChild) {
	  its::State leftStates = getStateVerifying (leftChild) ;
	  its::State rightStates = getStateVerifying (rightChild) ;
	  its::State satA = sat * leftStates ;
	  if (satA == State::null) {
	    out << "E a U b is false because none of your input states satisfies either a or b. \n";
	    return explain(sat,leftChild,out);
	  } else if (rightStates == its::State::null) {
	    out << "E a U b is false because no reachable states satisfy b. \n";
	    return explain(sat,rightChild,out);
	  } else {

	    its::Transition reachA = fixpoint(getNextRel()*leftStates + Transition::id,true);
	    its::State reachableA = reachA (satA);
	    its::Transition hasEG = fixpoint(getNextRel()*its::Transition::id,true);
	    its::State EGa = hasEG(reachableA);
	    out << "E a U b is false; \n";
	    if (EGa == its::State::null) {
	      out << "Although there are no cycles of the form EGa on your input states. \n";
	    } else {
	      out << "Because there exist cycles of the form EGa reachable from input states such as: \n";
	      model.printSomeStates(satA,out);
	      out << "TODO : report on lasso witness of SCC \n";
	      return satA;
	    }
	    its::State notAnotB = ( getNextRel() (reachableA) - leftStates ) - rightStates ;
	    if (notAnotB != State::null) {
	      its::path_t path = model.findPath(satA, notAnotB, reachableA, true);
	      out << "E a U b is false because all input states satisfying *a* lead to futures satisfying *not a* and *not b*. So formula : A a U !a&!b is true. \n";
	      out << "A minimal path of length " << path.getPath().size() << " from input states satisfying a to states satisfying !a and !b is:\n" ;
	      model.printPath(path,out);
	        
	    } else {
	      out << "E a U b is false because all paths satisfying a are finite (potential deadlocks !) and never traverse states satisfying b. \n";
	      return explain(satA,leftChild,out);
	    }
	  }
	} else {
	  std::cerr << "ERROR : EU has no children" << std::endl; 
	  assert(false);
	}
      }
      break;
    }
  case Ctlp_EG_c: 
    // start with states satisfying f
    // then remove states that are not a predescessor of a state in the set
    // EG f <->  ( f & pred )^* & f      
//       result = fixpoint (  
// 			   ( getPredRel()   
// 			     + ( getReachable() -  (getPredRel() (getReachable())) ) // i.e. add dead states that verify f
// 			     ) * its::Transition::id ) (leftStates) ; 
    if (formIsTrue && leftChild) {
      its::State leftStates = getStateVerifying (leftChild) ;
      
      its::Transition reachA = fixpoint(getNextRel()*leftStates + Transition::id,true);
      its::State reachableA = reachA (satF);
      // This lockstep algorithm identifies SCCs (w/o prefix or suffix) with a good overall complexity.
      // See papers on "A best symbolic SCC algorithm" by M. Vardi et. al. 
      its::Transition hasEG = fixpoint( (getPredRel()*its::Transition::id) &  (getNextRel()*its::Transition::id),true);
      its::State SCCa = hasEG(reachableA);
      
      its::path_t path = model.findPath(satF,SCCa, getReachable(), true);
      // length of path is a prefix to the lasso, a shortest path to an SCC.
      // compute SCCs really reachable from witness arrival states.
      
      // reachableA = reachA (path.getFinal());
      // SCCa = hasEG(reachableA);
      
      out << " EG a is true because following path leads from input states satisfying a to states satisfying a that belong to cycle(s) of a:\n ";
      model.printPath(path,out,true);
      out << std::endl;
      out << "Some states of an SCC reachable from these final states :\n";
      model.printSomeStates(SCCa,out);
      out << std::endl;
      

      its::path_t cycle = model.findCycle(path.getFinal(), SCCa);
      out << "A cycle :\n";
      model.printPath(cycle,out,true);
      out << std::endl;
      return path.getInit();
    } else if (leftChild) {
      its::State leftStates = getStateVerifying (leftChild) ;
      
      its::State satA = sat * leftStates ;
      if (leftStates == its::State::null) {
	out << "EG a  is false because no reachable states satisfy a. \n";
	return explain(sat,leftChild,out);
      } else if (satA == State::null) {
	out << "EG a is false because none of your input states satisfies a. \n";
	return explain(sat,leftChild,out);
      } else {
	// either there are finite paths of a
	// or all behaviors reach !a
	its::Transition reachA = fixpoint(getNextRel()*leftStates + Transition::id,true);
	its::State reachableA = reachA (satA);
	
	its::State deadlocks =  getReachable() - getPredRel() ( getReachable());
	its::State deadA = deadlocks * reachableA;
	
	if (deadA != its::State::null) {
	  out << "EG a is false; However, there are reachable deadlocks along paths satisfying a continuously.";
	  its::path_t path = model.findPath(satA, deadA, reachableA, true);
	  model.printPath(path,out);
	  return path.getInit();
	} else {
	  out << "EGa is false, since AF!a is true. From input states satisfying a, an example shortest trace to states satisfying !a is:";
	  its::State notA = getReachable() - leftStates;
	  
	  its::path_t path = model.findPath(satA, notA, reachableA, true);
	  model.printPath(path,out);	  
	  return path.getInit();
	} 
      }
    } else {
      std::cerr << "ERROR : EG has no children" << std::endl; 
      assert(false);
    }

    break;
      
    case Ctlp_Cmp_c: 
      {
	// Forward CTL specific : compare a formula to false or true
	// i.e. check whether a set is empty or not. return State::one to indicate truth, and State::null to indicate false.
	bool invertVerdict = false;
	its::State leftStates = getStateVerifying (leftChild) ;
	if (Ctlp_FormulaReadCompareValue(ctlFormula) == 0)
	  invertVerdict =true;
	formIsTrue = (invertVerdict && leftStates == State::null) || (!invertVerdict && leftStates!=State::null);
	if (formIsTrue) {
	  out << " is true in ";
	  if (sat.nbStates()==1) {
	    out << "the single input state :\n" ;
	    model.printSomeStates(sat,out);
	  } else {
	    out << satF.nbStates() <<  " state(s) out of "<< sat.nbStates() << " input state(s).\n";
	  }
	} else {
	  out << " is false in all " << sat.nbStates() << " input state(s).\n"  ;
	}
	return explain(getReachable(),leftChild,out);
	
	break;
      }
  case Ctlp_Init_c:
      // cast to constant homomorphism
    if (formIsTrue) {
      out << "Because some input states are initial states.\n";
      return satF;
    } else {
      out << "Because none of the input states are initial states.\n";
      return sat;
    }
    break;
  case Ctlp_FwdU_c:
      /** From Vis source documentation :
       *							    t
       ** E[p U q]      = lfp Z[q V (p ^ EX(Z))]   :   p p ... p q
       ** FwdUntil(p,q) = lfp Z[p V EY(Z ^ q)]     :		    pq q q ... q
       */
      /**
       * In other words, start from states satisfying p, then add successors satisfying q to fixpoint
       * FwdUntil(p,q) =  ( q & Next  + Id)^* & p
       */
      // test for trivial reachability case
if (Ctlp_FormulaReadLeftChild(ctlFormula) &&
    Ctlp_FormulaReadType(Ctlp_FormulaReadLeftChild(ctlFormula)) ==
 	  Ctlp_Init_c &&
	   Ctlp_FormulaReadRightChild(ctlFormula) &&
 	  Ctlp_FormulaReadType(Ctlp_FormulaReadRightChild(ctlFormula)) ==
 	  Ctlp_TRUE_c ) {
// 	// cast to constant hom
// 	result = getReachable() ;
	 if (formIsTrue) {
	   out << "This subformula computes all reachable states, and there are reachable states satisfying your other criteria.\n";
	   out << "A shortest path from an initial state to a state satisfying your property is\n";
	   its::path_t path = model.findPath(getInitialState(), sat, trueF, true);
	   model.printPath(path,out,true);	  
	   return path.getFinal();
	 } else {
	   out << "This subformula computes all reachable states, and there are no reachable states satisfying your other criteria.\n";
	   out << "No counter-example trace can be provided.\n";
	   return sat;
	 }
	 break;
       }
      // the real case
      // FwdUntil(p,q) holds at any state "t", such that there exists a path through "t" from some state at which
      // p holds, and q holds at all states before "t" on the path.
       if (formIsTrue) {
	 out << "This subformula computes reachability under FwdU constraint, and there are reachable states satisfying your other criteria.\n";
	 out << "A shortest path satisfying FwdU from an input initial state to a state satisfying your property is\n";
	 its::State leftStates = getStateVerifying (leftChild) ;
	 its::path_t path = model.findPath(leftStates, sat, trueF, true);
	 model.printPath(path,out,true);
	 explain(path.getInit(),leftChild,out);	   
	 return path.getFinal();
       } else {
	 out << "This subformula computes reachable states under FwdU, and there are no such reachable states satisfying your other criteria.\n";
	 out << "No counter-example trace can be provided.\n";
	 return sat;
       }

//       result = (getNextRel() +Transition::id) (fixpoint ( (rightStates * getNextRel()) + its::Transition::id ) ( leftStates )) ;
break;
  case Ctlp_FwdG_c:
   {
	// EH (p) is the subset of states verifying "p" that are reachable through a cycle in p
        // i.e. forward SCC hull of p states (keep p suffixes).
	// EH = fixpoint ( id * getNextRel() ) (p);

	// Reachable (p,q) : states that verify "q" reachable from states verifying "p and q" 
	// (while constantly verifying "q")
	// Reach (p,q) = fixpoint (  (q * next) + id )  (p * q)

	// FwdGlobal(p,q) = EH ( Reachable (p,q) )

	// states reachable by an infinite path of f
// 	result = fixpoint (  getNextRel() 
// 			     * ( fixpoint ( (rightStates * getNextRel()) + Transition::id  ) ( leftStates * rightStates)  )
// 			     ) ( getReachable() );



	// FwdGlobal(p,q) = EH ( Reachable (p,q) )
	// Start from states p, S = p
	// Keep only those satisfying q. S = S * q
	// Add any states satisfying q, q reachable from S. S = fix( Id +  q*Next ) (S)
	// Reduce to states in cycles + suffix thereof. S = fix ( Next * Id ) (S)
     break;
   }
 case Ctlp_EY_c:
      // exists yesterday : states that have a predescessor that verifies f
      // take states verifying f, then compute successors
      // EY f  <->  Next & f (reach)
//       result = getNextRel() ( leftStates ) ;
   break;
   
  default: 
      out << "Encountered unknown type of CTL formula\n";
  }
  return sat;
}


bool CTLChecker::hasSCCs () const {
  if (! scc_computed_) {
    scc_computed_ =true;
    its::Transition findscc = fixpoint( getNextRel() * Transition::id );
    State sccs = findscc.has_image(getReachable());
    if (sccs == State::null) {	   
      if (!beQuiet) std::cout << "Fast SCC detection found none." << std::endl;      
	  scc_ = false;	
    } else {
	  scc_ = true;
	}
  }
  return scc_;
}

its::State  CTLChecker::getStateVerifying (Ctlp_Formula_t *ctlFormula, bool need_exact) const {
#ifdef ITS_EXERCISE
    Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
    Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);
    switch (Ctlp_FormulaReadType(ctlFormula)) {
    case Ctlp_ID_c:
      {
      vLabel ap = vLabel(Ctlp_FormulaReadVariableName(ctlFormula)) + Ctlp_FormulaReadValueName(ctlFormula) ;
      return ctl::computeAtomicPredicate(ap, *this);
      }
    case Ctlp_Init_c:
      return ctl::computeInitial(*this);
    case Ctlp_TRUE_c:
      return ctl::computeTrue(*this); 
    case Ctlp_FALSE_c:
      return ctl::computeFalse(*this); 
    case Ctlp_NOT_c:
      return ctl::computeNot(leftChild,*this); 
    case Ctlp_AND_c:
      return ctl::computeAnd(leftChild,rightChild,*this); 
    case Ctlp_THEN_c:
      return ctl::computeImplies(leftChild,rightChild,*this); 
    case Ctlp_OR_c:
      return ctl::computeOr(leftChild,rightChild,*this); 
    case Ctlp_EX_c:
      return ctl::computeEX(leftChild,*this); 
    case Ctlp_EU_c: 
      return ctl::computeEU(leftChild,rightChild,*this);
    case Ctlp_EG_c: 
      return ctl::computeEG(leftChild,*this);
    case Ctlp_Cmp_c: 
      // Forward CTL specific : compare a formula to false or true
      // i.e. check whether a set is empty or not. return State::one to indicate truth, and State::null to indicate false.
      if (Ctlp_FormulaReadCompareValue(ctlFormula) == 0)
 	return (getStateVerifying(leftChild) == State::null ? State::one : State::null);
      else
	return (getStateVerifying(leftChild) == State::null ? State::null : State::one);
    default :
      throw "Unexpected case in Exercise mode.";
    }    
#else    
  its::Transition stop = Shom(GSDD::top);
  if (! beQuiet) {
  	std::cout << "Checking (exact) " << need_exact << " :" ;
  	Ctlp_FormulaPrint(vis_stdout,ctlFormula);
  	std::cout << std::endl ;
  }
  if (! need_exact) {
    its::State result ;

    Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
    Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);
    its::Transition leftHom, rightHom;    
    if (leftChild) {
      leftHom = getHomomorphism(leftChild);
    }
    if (rightChild) {	
      rightHom = getHomomorphism(rightChild);
    }

    // Now switch on the type of formula
    switch (Ctlp_FormulaReadType(ctlFormula)) {
      // Semantics remain the same as need_exact case for these formulas
    case Ctlp_TRUE_c:
    case Ctlp_FALSE_c:
    case Ctlp_EQ_c:
    case Ctlp_XOR_c:
    case Ctlp_THEN_c:
    case Ctlp_Init_c:
    case Ctlp_Cmp_c:
      break;


    case Ctlp_ID_c:
      {
	// basic case
	Transition sel =  getSelectorAP( Ctlp_FormulaReadVariableName(ctlFormula) , Ctlp_FormulaReadValueName(ctlFormula) );
	return sel.has_image(getReachable());
      }
    case Ctlp_NOT_c:
      if (leftHom != stop) {
	return  (! leftHom ).has_image(getReachable());
      }
      break;      
    case Ctlp_AND_c:
      if (rightHom != stop) {
	if ( rightHom.has_image(getReachable()) == State::null) {
	  return State::null;
	} 
      }
      if (leftHom != stop) {
	if ( leftHom.has_image(getReachable()) == State::null) {
	  return State::null;
	} 
      }
      break;

    case Ctlp_OR_c:
      if (leftHom != stop) {
	State img = leftHom.has_image(getReachable()) ;
	if ( img  != State::null) {
	  return img;
	} 
      }
      if (rightHom != stop) {
	State img = rightHom.has_image(getReachable()) ;
	if ( img  != State::null) {
	  return img;
	} 
      }
      if (leftHom == stop) {
	State img = getStateVerifying( leftChild , false) ;
	if ( img  != State::null) {
	  return img;
	} 
      }
      if (rightHom == stop) {
	State img = getStateVerifying( rightChild , false) ;
	if ( img  != State::null) {
	  return img;
	} 
      }
      return State::null;


    case Ctlp_EX_c:
      if (leftHom != stop) {
	return  (getPredRel() & leftHom).has_image(getReachable());
      } else {
	State img = getStateVerifying( leftChild , true) ;
	return getPredRel().has_image (img);
      }

    case Ctlp_EU_c: 
      {
      // start from states satisfying g, then add predescessors verifying f in a fixpoint.
      // f U g <-> ( f & pred + Id )^* & g
	if (rightHom != stop) {
	  State img = rightHom.has_image(getReachable()) ;
	  // Since img is included in (f & pred + Id)^* (img)
	  return img;
	} else {
	  State img = getStateVerifying(rightChild, false);
	  return img;
	}	
      }
    case Ctlp_EG_c: 
      {
      // start with states satisfying f
      // then remove states that are not a predescessor of a state in the set
      // EG f <->  ( f & pred )^* & f      
	if (leftHom != stop) {
	  State img = leftHom.has_image(getReachable()) ;
	  if (img == State::null) {
	    return img;
	  }
	}
	State leftStates = getStateVerifying(leftChild,true);
	its::State deadf = ( getReachable() -  (getPredRel() (getReachable())) )*leftStates ; // i.e. add dead states that verify f; 
	if (deadf != State::null) {
	  return deadf;
	} else {
	  return fixpoint ( getPredRel() * its::Transition::id , true).has_image (leftStates); 
	}
      }			   
 


    // case Ctlp_FwdU_c:
    //   /** From Vis source documentation :
    //    *							    t
    //    ** E[p U q]      = lfp Z[q V (p ^ EX(Z))]   :   p p ... p q
    //    ** FwdUntil(p,q) = lfp Z[p V EY(Z ^ q)]     :		    pq q q ... q
    //    */
    //   /**
    //    * In other words, start from states satisfying p, then add successors satisfying q to fixpoint
    //    * FwdUntil(p,q) =  ( Next & (q * id)  + Id)^* ( p )
    //    */
    //   // test for trivial reachability case
    //   if (Ctlp_FormulaReadLeftChild(ctlFormula) &&
    // 	  Ctlp_FormulaReadType(Ctlp_FormulaReadLeftChild(ctlFormula)) ==
    // 	  Ctlp_Init_c &&
    // 	  Ctlp_FormulaReadRightChild(ctlFormula) &&
    // 	  Ctlp_FormulaReadType(Ctlp_FormulaReadRightChild(ctlFormula)) ==
    // 	  Ctlp_TRUE_c ) {
    // 	// cast to constant hom
    // 	result = getReachable() ;
    // 	break;
    //   }

    //   // the real case
    //   // FwdUntil(p,q) holds at any state "t", such that there exists a path through "t" from some state at which
    //   // p holds, and q holds at all states before "t" on the path.
    //   if (rightHom == stop ) {
    // 	result = fixpoint ( (getNextRel() & (rightStates * its::Transition::id)) + its::Transition::id, true ) ( leftStates ) ;
    //   } else {
    // 	its::Transition t = fixpoint ( ( getNextRel() & rightHom ) + its::Transition::id, true );
    // 	//	std::cerr << t << std::endl; 
    // 	result = t ( leftStates ) ;
    //   }
    //   break;
    // case Ctlp_FwdG_c:
    //   {
    // 	  // from original forward CTL paper :
    // 	  // EH (p) = gfp Z [p ^ Img (Z)]
    // 	  // Reachable (p; q) = FwdUntil (p; q) ^ q
    // 	  // FwdGlobal (p; q) = EH (Reachable (p; q))
	  
    // 	  // so, EH (p) = ( Next * id )^* (p)
    // 	  // Reachable(p,q) = FwdUntil(p;q) ^q
    // 	  // and  FwdUntil(p,q) =  ( Next & (q * id)  + Id)^* ( p )
    // 	  // Reachable(p,q) = (( Next & (q * id)  + Id)^* ( p )) * q
    // 	  // FwdGlobal (p; q) = ( Next * id )^* ((( Next & (q * id)  + Id)^* ( p )) * q)
    // 	//	result = fixpoint ( (getNextRel() 
    // 	//			     +   (getReachable() -  (getPredRel() (getReachable())))) * Transition::id )  
    // 		// FwdUntil(p,q)
    // 	//	(fixpoint ( (getNextRel() & (rightStates * its::Transition::id)) + its::Transition::id ) ( leftStates  
    // 		// ^q 
    // 	//												   * rightStates));
		
	
    // 	// EH (p) is the subset of states verifying "p" that are reachable through a cycle in p
    // 	// EH = fixpoint ( p * getNextRel() ) (getReachable);

    // 	// Reachable (p,q) : states that verify "q" reachable from states verifying "p and q" 
    // 	// (while constantly verifying "q")
    // 	// Reach (p,q) = fixpoint (  (q * next) + id )  (p * q)

    // 	// FwdGlobal(p,q) = EH ( Reachable (p,q) )

    // 	its::State reachpq ;
    // 	if (leftHom != stop && rightHom != stop) {
    // 	  reachpq = (leftHom & rightHom) ( getReachable() );
    // 	} else {
    // 	  reachpq = leftStates * rightStates;
    // 	}
    // 	if ( rightHom ==stop) {
    // 	  reachpq = fixpoint ( (rightStates * getNextRel()) + Transition::id, true) (reachpq)  ;
    // 	} else {
    // 	  reachpq = fixpoint ( (rightHom & getNextRel()) + Transition::id, true) (reachpq)  ;
    // 	}
    // 	its::State dead = getReachable() -  (getPredRel() (getReachable()))  ; // i.e. add dead states that verify f
    // 	// states reachable by an infinite path of f
    // 	its::State dpq = dead * reachpq;
    // 	// if (dpq != its::State::null) {
    // 	//   result = dpq;
    // 	// } else {
    // 	  its::Transition fix = fixpoint ( getNextRel()  * its::Transition::id, true);
    // 	  //	  std::cout << "going for has image to compute gfp of "<< fix <<  std::endl;
    // 	  // result = fix.has_image(reachpq) + dpq;	 
    // 	   result =  fix ( reachpq ) + dpq;
    // 	//      }


    // 	// FwdGlobal(p,q) = EH ( Reachable (p,q) )
    // 	// Start from states p, S = p
    // 	// Keep only those satisfying q. S = S * q
    // 	// Add any states satisfying q, q reachable from S. S = fix( Id +  q*Next ) (S)
    // 	// Reduce to states in cycles + suffix thereof. S = fix ( Next * Id ) (S)
    // 	break;
    //   }
    // case Ctlp_EY_c:
    //   // exists yesterday : states that have a predescessor that verifies f
    //   // take states verifying f, then compute successors
    //   // EY f  <->  Next & f (reach)
    //   result = getNextRel() ( leftStates ) ;
    //   break;

    case Ctlp_FwdG_c:
      {


	break;
      }
      
      case Ctlp_EY_c:
    case Ctlp_FwdU_c:
      std::cerr << "Inexact computation unsupported for formula. Fall back to exact computation."<<std::endl; 
      break;
    default: 
      fail("Encountered unknown type of CTL formula\n");      
      break;
    }
  }

  its::State result;
  ctl_statecache_it it = ctl_statecache.find(ctlFormula);
  if ( it == ctl_statecache.end() ) {
    // A miss 
    // invoke recursive procedures
    its::State leftStates, rightStates;
    its::Transition leftHom, rightHom;    
    {
      // test necessary conditions
      if ( Ctlp_FormulaReadType(ctlFormula) == Ctlp_AND_c) {
	Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);
	rightHom = getHomomorphism(rightChild);
	Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
	if (rightHom != stop && Ctlp_FormulaReadType(leftChild) == Ctlp_Init_c) {
	  return rightHom ( getInitialState());
	}
	// Explore right child will "luck" out pretty well due to rewriting of FwdU and FwdG
	rightStates = getStateVerifying (rightChild);
	if (rightStates == State::null) { 
	  return State::null;
	} 
      } else if (Ctlp_FormulaReadType(ctlFormula) == Ctlp_Cmp_c) {
	Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
	// Forward CTL specific : compare a formula to false or true
	// i.e. check whether a set is empty or not. return State::one to indicate truth, and State::null to indicate false.
	leftStates = getStateVerifying(leftChild,false);
	if (Ctlp_FormulaReadCompareValue(ctlFormula) == 0) {
	  return (leftStates == State::null ? State::one : State::null);
	} else {
	  return (leftStates == State::null ? State::null : State::one);
	}
      } else if (Ctlp_FormulaReadType(ctlFormula) == Ctlp_NOT_c) {
	Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
	if (Ctlp_FormulaReadType(leftChild) == Ctlp_NOT_c) {
	  return getStateVerifying(Ctlp_FormulaReadLeftChild(leftChild), need_exact);
	}
      }
    }
    // Handle left child
    {
      Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
      if (leftChild) {
	leftHom = getHomomorphism(leftChild);
	if (leftHom == stop) {
	  leftStates = getStateVerifying (leftChild);
	} else {
	  leftStates = leftHom ( getReachable()) ; 
	  // if (leftStates !=  getStateVerifying (leftChild)) {
	  //   Ctlp_FormulaPrint(vis_stdout,leftChild);	    
	  //   std::cerr << "Incoherent Hom vs States (left)" << std::endl;
	  //   throw "error";
	  // }
	}
	if (leftStates == getReachable()) {
	  leftHom = Transition::id;
	}

      }
    }
    // Handle right child
    if (rightStates == State::null) {
      Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);
      if (rightChild) {	
	rightHom = getHomomorphism(rightChild);
	if (rightHom == stop) {
	  rightStates = getStateVerifying (rightChild);
	} else {
	  rightStates = rightHom ( getReachable()) ;
	  // if (rightStates !=  getStateVerifying (rightChild)) {
	  //   Ctlp_FormulaPrint(vis_stdout,rightChild);	    
	  //   std::cerr << "Incoherent Hom vs States (right)" << std::endl;
	  //   throw "error";
	  // }

	}
	if (rightStates == getReachable()) {
	  rightHom = Transition::id;
	}
      }      
    }
    //    std::cerr << "Translating CTL formula : " ;
    //    Ctlp_FormulaPrint(vis_stderr, ctlFormula);
    //    std::cerr << std::endl;

    // Now switch on the type of formula
    switch (Ctlp_FormulaReadType(ctlFormula)) {
    case Ctlp_ID_c:
      {
	// basic case
	Transition sel =  getSelectorAP( Ctlp_FormulaReadVariableName(ctlFormula) , Ctlp_FormulaReadValueName(ctlFormula) );
	result = sel (getReachable());
	break;
      }
    case Ctlp_TRUE_c:
      result = getReachable();
      break;

    case Ctlp_FALSE_c:
      result = State::null; 
      break;

    case Ctlp_NOT_c:
      result = getReachable() - leftStates ; 
      break;
      
    case Ctlp_AND_c:
      result = leftStates *  rightStates;
      break;

    case Ctlp_EQ_c:
      assert(false);
//      result = mdd_xnor(leftMdd, rightMdd); 
      break;

    case Ctlp_XOR_c:
      assert(false);
//      result = mdd_xor(leftMdd, rightMdd); break;
      break;

    case Ctlp_THEN_c:
      // A => B    <->  not A or B
      result = (getReachable() - leftStates) + rightStates ;
      break;

    case Ctlp_OR_c:
      result = leftStates  +  rightStates ;

      break;

    case Ctlp_EX_c:
      
      result = getPredRel() (leftStates);
      break;

    case Ctlp_EU_c: 
      {
      // start from states satisfying g, then add predescessors verifying f in a fixpoint.
      // f U g <-> ( f & pred + Id )^* & g
	if (leftHom == stop) {
	  result = fixpoint ( (leftStates * getPredRel()) + its::Transition::id, true )  (rightStates) ;
	} else {
	  result = fixpoint ( (leftHom & getPredRel()) + its::Transition::id, true )  (rightStates) ;
	}
      break;
      }
    case Ctlp_EG_c: 
      {
      // start with states satisfying f
      // then remove states that are not a predescessor of a state in the set
      // EG f <->  ( f & pred )^* & f      
      its::State deadf = ( getReachable() -  (getPredRel() (getReachable())) )*leftStates ; // i.e. add dead states that verify f; 
      its::State deadg ;// i.e. add  predecessors of dead states satisfying f
      if (leftHom == stop) {
	deadg =  fixpoint( (leftStates * getPredRel()) + Transition::id ) (deadf)  ; 
      } else {
	// trigger rewriting ( sel&next + id) ^* potentially
	deadg =  fixpoint( (leftHom & getPredRel()) + Transition::id ) (deadf)  ; 
      }
      bool sccs = hasSCCs();
      if (! sccs ) {
	result = deadg;
      } else {
	if (leftHom == stop) {
	  its::Transition egfix = fixpoint ( getPredRel() * its::Transition::id, true); 
	  its::State img = egfix.has_image(leftStates);
	  if (img == State::null) {
	    if (! beQuiet) std::cout << "Fast SCC detection found none." << std::endl;
	    result = deadg ;
	  } else {
	    result = egfix (leftStates) + deadg; 
	  }
	} else {
	  its::Transition egfix = fixpoint ( ( leftHom & getPredRel() ), true);
	  its::State img = egfix.has_image(leftStates);
	  if (img == State::null) {
	    if (! beQuiet) std::cout << "Fast SCC detection found none." << std::endl;
	    result = deadg ;
	  } else {
	    result = egfix (leftStates)+ deadg; 
	  }
	}
      }
      break;
      }			   
    case Ctlp_Cmp_c: {
      // Forward CTL specific : compare a formula to false or true
      // i.e. check whether a set is empty or not. return State::one to indicate truth, and State::null to indicate false.
      if (Ctlp_FormulaReadCompareValue(ctlFormula) == 0)
 	result = (leftStates == State::null ? State::one : State::null);
      else
	result = (leftStates == State::null ? State::null : State::one);
      break;
    }
    case Ctlp_Init_c:
      // cast to constant homomorphism
      result = getInitialState() ;
      break;
    case Ctlp_FwdU_c:
      /** From Vis source documentation :
       *							    t
       ** E[p U q]      = lfp Z[q V (p ^ EX(Z))]   :   p p ... p q
       ** FwdUntil(p,q) = lfp Z[p V EY(Z ^ q)]     :		    pq q q ... q
       */
      /**
       * In other words, start from states satisfying p, then add successors satisfying q to fixpoint
       * FwdUntil(p,q) =  ( Next & (q * id)  + Id)^* ( p )
       */
      // test for trivial reachability case
      if (Ctlp_FormulaReadLeftChild(ctlFormula) &&
	  Ctlp_FormulaReadType(Ctlp_FormulaReadLeftChild(ctlFormula)) ==
	  Ctlp_Init_c &&
	  Ctlp_FormulaReadRightChild(ctlFormula) &&
	  Ctlp_FormulaReadType(Ctlp_FormulaReadRightChild(ctlFormula)) ==
	  Ctlp_TRUE_c ) {
	// cast to constant hom
	result = getReachable() ;
	break;
      }

      // the real case
      // FwdUntil(p,q) holds at any state "t", such that there exists a path through "t" from some state at which
      // p holds, and q holds at all states before "t" on the path.
      if (rightHom == stop ) {
	if (rightStates == getReachable()) {
	  // FwdUntil(p, TRUE) = q holds everywhere, so its just reachable states from p
	  result = fixpoint ( getNextRel() + its::Transition::id, true ) ( leftStates ) ;
	} else {
	  result = fixpoint ( (getNextRel() & (rightStates * its::Transition::id)) + its::Transition::id, true ) ( leftStates ) ;
	}
      } else {
	its::Transition t = fixpoint ( ( getNextRel() & rightHom ) + its::Transition::id, true );
	//	std::cerr << t << std::endl; 
	result = t ( leftStates ) ;
      }
      break;
    case Ctlp_FwdG_c:
      {
	  // from original forward CTL paper :
	  // EH (p) = gfp Z [p ^ Img (Z)]
	  // Reachable (p; q) = FwdUntil (p; q) ^ q
	  // FwdGlobal (p; q) = EH (Reachable (p; q))
	  
	  // so, EH (p) = ( Next * id )^* (p)
	  // Reachable(p,q) = FwdUntil(p;q) ^q
	  // and  FwdUntil(p,q) =  ( Next & (q * id)  + Id)^* ( p )
	  // Reachable(p,q) = (( Next & (q * id)  + Id)^* ( p )) * q
	  // FwdGlobal (p; q) = ( Next * id )^* ((( Next & (q * id)  + Id)^* ( p )) * q)
	//	result = fixpoint ( (getNextRel() 
	//			     +   (getReachable() -  (getPredRel() (getReachable())))) * Transition::id )  
		// FwdUntil(p,q)
	//	(fixpoint ( (getNextRel() & (rightStates * its::Transition::id)) + its::Transition::id ) ( leftStates  
		// ^q 
	//												   * rightStates));
		
	
	// EH (p) is the subset of states verifying "p" that are reachable through a cycle in p
	// EH = fixpoint ( p * getNextRel() ) (getReachable);

	// Reachable (p,q) : states that verify "q" reachable from states verifying "p and q" 
	// (while constantly verifying "q")
	// Reach (p,q) = fixpoint (  (q * next) + id )  (p * q)

	// FwdGlobal(p,q) = EH ( Reachable (p,q) )
	its::State reachpq ;

	its::State dead = getReachable() -  (getPredRel() (getReachable()))  ; // i.e. add dead states that verify f

	bool sccs = hasSCCs ();
	if (! sccs) {
	  dead = dead * rightStates ;
	  if (dead == State::null) {
	    std::cout << "dead was empty" << std::endl;
	    return result;
	  }
	}

	if (leftHom != stop && rightHom != stop) {
	  reachpq = (leftHom & rightHom) ( getReachable() );
	} else {
	  reachpq = leftStates * rightStates;
	}

	if (rightHom ==stop) {
	    reachpq = fixpoint ( (rightStates * getNextRel()) + Transition::id, true) (reachpq)  ;
	} else {
	    reachpq = fixpoint ( (rightHom & getNextRel()) + Transition::id, true) (reachpq)  ;
	}
	
	if (! sccs) {
	  result = dead * reachpq;
	} else {
	  // states reachable by an infinite path of f
	  result = fixpoint ( getNextRel() 
			    * its::Transition::id 
			    , true) ( reachpq ) + (dead * reachpq);
	
	  // FwdGlobal(p,q) = EH ( Reachable (p,q) )
	  // Start from states p, S = p
	  // Keep only those satisfying q. S = S * q
	  // Add any states satisfying q, q reachable from S. S = fix( Id +  q*Next ) (S)
	  // Reduce to states in cycles + suffix thereof. S = fix ( Next * Id ) (S)
	}	
	break;
      }
    case Ctlp_EY_c:
      // exists yesterday : states that have a predescessor that verifies f
      // take states verifying f, then compute successors
      // EY f  <->  Next & f (reach)
      result = getNextRel() ( leftStates ) ;
      break;
      
    default: 
      fail("Encountered unknown type of CTL formula\n");
    }

    // std::cerr << "Obtained Homomorphism : " << result << std::endl;

    
  } else {
    result = it->second;
  }  
  return result;
#endif // ITS_EXERCISE
}

its::Transition CTLChecker::getAtomicPredicate (Label ap) const {
  its::Transition pred = model.getPredicate(ap);
//  std::cout << pred << std::endl;
  return pred;
}

  its::Transition CTLChecker::getSelectorAP (Label apname, Label val) const {

  vLabel predicate = apname +  val;
  its::Transition pred = model.getPredicate(predicate);
//  std::cout << pred << std::endl;
  return pred;

// BEFORE PREDICATE API INTRODUCED WAS :
/**
  //  std::cerr << "asked for selector AP for :" << apname << std::endl;
  pType type =  model.getInstance()->getType();
  labels_t labs = type->getTransLabels();
  if ( find (labs.begin() , labs.end() , apname) == labs.end() ) {
    std::cerr << "Your atomic proposition identifier \"" << apname << "\" does not correspond to a known predicate. Check that it is the name of a place in your Petri net, or relevant with respect to your ITS type declaration." << std::endl;
    std::cerr << "Error is fatal, sorry." << std::endl;
    exit(1);
  }
  labels_t touse ;
  {
    touse.push_back(apname);
    
    Transition t = type->getSuccs (touse);
    if (! t.is_selector() ) {
      std::cerr << "Your atomic proposition identifier " << apname << " does not correspond to a state based predicate (it is not a selector). Check that it is the name of a place in your Petri net, or relevant with respect to your ITS type declaration." << std::endl;
      std::cerr << "Error is fatal, sorry." << std::endl;
      exit(1);
    }
    return t;
  }

  return Transition::id;
*/
}

its::Transition CTLChecker::getPredRel (its::State envelope) const
{
  if (isfairtime) {
    if (predRel_ == Transition::id || envelope != State::null) {
      
      State reach ;
      if (envelope == State::null) {
	reach= getReachable();
      } else {
	reach = envelope;
      }
      Transition nextRel = getNextRel();
      if (nextRel == Transition::null) {
	return Transition::null;
      }
      Transition inv = nextRel.invert(reach);
      bool isExact = ( inv(reach) - reach == State::null );
      if (isExact) {
	predRel_ = inv;
	std::cerr << "Reverse transition relation is exact ! Faster fixpoint algorithm enabled. \n" ;
      } else {
	predRel_ = inv * reach;
	std::cerr << "Reverse transition relation is NOT exact ! Intersection with reachable at each step enabled. \n" ;
      }
    }
    return predRel_;
  } else {
    return model.getPredRel(envelope);
  }
}

its::Transition CTLChecker::getNextRel () const
{
  if (isfairtime) {
    Transition trans = model.getInstance()->getType()->getLocals();

    Transition elapse = model.getElapse(); 
    if (elapse != Transition::id) {
//      return trans + elapse;
      return  fixpoint(elapse + Transition::id) & trans;
    }
    return trans;
  } else {
    return model.getNextRel();
  }
}

its::State CTLChecker::getReachable () const {
  // this call is cached in ITSModel
  return model.computeReachable();
}

its::State CTLChecker::getInitialState () const {
  // this call is cached in ITSModel

  if (isfairtime) {
    return fixpoint(model.getElapse() + Transition::id) ( model.getInitialState() );
  } else {
    return model.getInitialState();
  }
}

its::State CTLChecker::getReachableDeadlocks () const {
  Transition nextRel = getNextRel();
  if (nextRel == Transition::null) {
    return getReachable();
  }
  return getReachable() - ( nextRel.invert(getReachable()) ( getReachable()));
}

its::State CTLChecker::getReachableTimelocks () const {
  Transition elapse = model.getElapse(); 
  if (elapse != Transition::id) {
  	// gfp : instersect id
  	Transition findSCC = fixpoint(elapse * Transition::id, true);
  	// due to time event nature, these states are necessarily images of themselves by "elapse".
  	// i.e. SCC reduced to one state each
  	State inSCC = findSCC(getReachable()) ;
  	// No elapse ! Just local private events
  	Transition trans = model.getInstance()->getType()->getLocals();
  	// transition dead states : no successor by normal transitions
  	State trDead;
  	if (trans == Transition::null) {
   	  trDead = getReachable();
    } else {
	  trDead =  getReachable() - ( trans.invert(getReachable()) ( getReachable()));
	}
  	// remove states with other outputs
  	return inSCC * trDead;
  } else {
  	std::cerr << "No time elapse event found when looking for timelocks."<< std::endl;
  	return State::null;
  }
}
