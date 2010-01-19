


#include "ctlCheck.hh"
#include "visfd.h"

using namespace its;




its::State  CTLChecker::getStateVerifying (Ctlp_Formula_t *ctlFormula) const {
  its::State result;
  ctl_statecache_it it = ctl_statecache.find(ctlFormula);
  if ( it == ctl_statecache.end() ) {
    // A miss 
    // invoke recursive procedures
    its::State leftStates, rightStates;
    // Handle left child
    {
      Ctlp_Formula_t *leftChild = Ctlp_FormulaReadLeftChild(ctlFormula);
      if (leftChild) {
	leftStates = getStateVerifying (leftChild);
      }
    }
    // Handle right child
    {
      Ctlp_Formula_t *rightChild = Ctlp_FormulaReadRightChild(ctlFormula);
      if (rightChild) {
	rightStates = getStateVerifying (rightChild);
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
	Transition sel =  getSelectorAP( Ctlp_FormulaReadVariableName(ctlFormula) );
	if ( ! strcmp (Ctlp_FormulaReadValueName(ctlFormula), "0") )
	  sel = ! sel;
	result = sel (getReachable());
	break;
      }
    case Ctlp_TRUE_c:
      // Any reachable state verifies true
      result = State::null ; // TODO
      break;

    case Ctlp_FALSE_c:
      // No state verifies false
      result = State::null; 
      break;

    case Ctlp_NOT_c:
      // defined as reachable states, except those that verify 
      // the property (left child)
      // use set set difference
      result = State::null ; 
      break;
      
    case Ctlp_AND_c:
      // states that verify a and b
      // intersection of the two sets
      result = State::null;
      break;

    case Ctlp_EQ_c:
      // should not happen
      assert(false);
      break;

    case Ctlp_XOR_c:
      // should not happen
      assert(false);
      break;

    case Ctlp_THEN_c:
      // A => B    <->  not A or B
      result = State::null ;
      break;

    case Ctlp_OR_c:
      // A or B : use union
      result = State::null ;

      break;

    case Ctlp_EX_c:
      // predescessors of states verifying A
      result = State::null;
      break;

    case Ctlp_EU_c: 
      {
      // start from states satisfying g, then add predescessors verifying f in a fixpoint.
	result = State::null ;
	break;
      }
    case Ctlp_EG_c: 
      // start with states satisfying f
      // then remove states that are not a predescessor of a state in the set
      result = State::null ; 
      break;
			   
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
       * FwdUntil(p,q) =  ( q & Next  + Id)^* & p
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
      result = fixpoint ( (rightStates * getNextRel()) + its::Transition::id ) ( leftStates ) ;
      break;
    case Ctlp_FwdG_c:
      {
	// EH (p) is the subset of states verifying "p" that are reachable through a cycle in p
	// EH = fixpoint ( p * getNextRel() ) (getReachable);

	// Reachable (p,q) : states that verify "q" reachable from states verifying "p and q" 
	// (while constantly verifying "q")
	// Reach (p,q) = fixpoint (  (q * next) + id )  (p * q)

	// FwdGlobal(p,q) = EH ( Reachable (p,q) )

	// states reachable by an infinite path of f
	result = fixpoint (  getNextRel() 
			     * ( fixpoint ( (rightStates * getNextRel()) + Transition::id  ) ( leftStates * rightStates)  )
			     ) ( getReachable() );
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
}

its::Transition CTLChecker::getSelectorAP (Label apname) const {
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
      std::cerr << "Your atomic proposition identifier " << apname << " does not correspond to a state based predicate. Check that it is the name of a place in your Petri net, or relevant with respect to your ITS type declaration." << std::endl;
      std::cerr << "Error is fatal, sorry." << std::endl;
      exit(1);
    }
    return t;
  }

  return Transition::id;
}

its::Transition CTLChecker::getPredRel () const
{
    if (predRel == Transition::null) {
      State reach = getReachable();
      Transition rel = model.getNextRel().invert(reach);
      bool isExact = ( rel(reach) - reach == State::null );
      if (isExact) {
	predRel = rel;
	std::cout << "Reverse transition relation is exact ! Faster fixpoint algorithm enabled. \n" ;
      } else {
	predRel = rel * reach;
	std::cout << "Reverse transition relation is NOT exact ! Intersection with reachable at each step enabled. \n" ;
      }
    }
    return predRel;
}

its::Transition CTLChecker::getNextRel () const
{
  return model.getNextRel() ;
}

its::State CTLChecker::getReachable () const {
  // this call is cached in ITSModel
  return model.computeReachable();
}

its::State CTLChecker::getInitialState () const {
  // this call is cached in ITSModel
  return model.getInitialState();
}
