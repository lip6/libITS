#include "sogIts.hh"

using namespace its;


// #define trace std::cerr
#define trace if (0) std::cerr



// Atomic properties handling primitives
// return a selector corresponding to the boolean formula over AP encoded as a bdd.
Transition sogIts::getSelector(bdd aps) const {
  pType type = model.getInstance()->getType();
  return getSelector(aps,type);
}


// Other version to act on any given type, instead of the main instance of the Model object
// return a selector corresponding to the boolean formula over AP encoded as a bdd.
its::Transition sogIts::getSelector(bdd aps, its::pType type) const {
  formCache_it it ;
  if ( formulaCache.find(it,aps.id()) )
    // cache hit
    return it->second;

  if ( aps == bddtrue ) {
    return Transition::id ;
  } else if (aps == bddfalse ) {
    return Transition::null;
  } else {
    // a "real" node
    int bvar = bdd_var(aps);
    Label prop = apOrder_.getLabel(bvar);

    Transition hcond ;
//    int len =type->getName().size();
    if (atoms.size() > 0) {
    	bool seen = false;
    	for (const auto & ap : atoms) {
    		if (ap.getName() == prop) {
    			const std::string & body = ap.getPred();
    	        char buff [body.size()+1];
    	        strcpy(buff,body.c_str());
    	        hcond = type->getPredicate(buff);
    	        seen = true;
    			break;
    		}
    	}
    	if (!seen) {
    		std::cerr << "Unknown atomic proposition " << prop << std::endl;
    		exit(1);
    	}

    } else if (! isPlaceSyntax ) {
      char buff [prop.size()+1];
      strcpy(buff,prop.c_str());
      hcond = type->getPredicate(buff);
    } else {
      char buff [prop.size()+3];
      strcpy(buff,prop.c_str());
      strcat(buff,"=1");
      hcond = type->getPredicate(buff);
      // Used to be (before introduction of getPredicate in ITSModel)
//       labels_t tau;
//       tau.push_back(prop);
//       hcond = type->getSuccs(tau);
    }

    trace << "aps = " << aps  << std::endl;
    //      std::cerr << "Type = " << * (getInstance()->getType())  << std::endl;
    trace << "prop = " << prop << std::endl ;
    trace << "hcond = "  << hcond << std::endl ;

    Transition ret ;
    if ( bdd_high(aps) == bdd_low(aps) ) {
      // This AP is a don't care in this part of the boolean formula
      ret = getSelector(bdd_high(aps));
    } else {
      ret = ITE(hcond, getSelector(bdd_high(aps)),  getSelector(bdd_low(aps)) );
    }
    formulaCache.insert(it, aps.id());
    it->second = ret;

    trace << "Produced transition :" << ret << std::endl;
    return ret;
  }
}


// Saturate the provided states, while preserving the truth value of "cond"
// The truth value of cond need not be homogeneous in the provided states.
// fixpoint ( hcond & locals() + id ) ( hcond(s) )
// Where hcond represents getSelector(cond)
its::State sogIts::leastPostTestFixpoint ( State init, bdd cond ) const {
  Transition hcond = getSelector(cond);
  Transition hnext = getNextRel();

  Transition sat = fixpoint ( (hcond &  hnext) + Transition::id , true ) & hcond;
  trace << "Saturate (post) least fixpoint under conditions : " << sat << std::endl;
  return sat(init);
}

// Saturate the provided states, while ensuring that only states with
// "cond" or their successors are kept
// fixpoint ( locals() & hcond + id ) (s)
its::State sogIts::leastPreTestFixpoint ( its::State init, bdd cond ) const {
  Transition hcond = getSelector(cond);
  Transition hnext = getNextRel();

  Transition sat = fixpoint ( ( hnext & hcond ) + Transition::id , true );

  trace << "Saturate (pre) least fixpoint under conditions : " << sat << std::endl;
  State res =  sat(init);

  return res;
}


// Find successors of states satisfying "cond" in the provided aggregate "init"
State  sogIts::succSatisfying ( its::State init, bdd cond) const {
  return (getNextRel()  & getSelector(cond)) (init);
}

// Return the set of divergent states in a set, using  (hcond & next) as transition relation
State sogIts::getDivergent (State init, bdd cond) const {
  // build separately from application to avoid gc...
  Transition fix = fixpoint ( (getSelector(cond) & model.getNextRel()) * Transition::id , true );
  return fix (init);
}

