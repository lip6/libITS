#include "ITSModel.hh"

// For factory role
#include "petri/PNetType.hh"
#include "petri/TPNetType.hh"
#include "composite/Composite.hh"
#include "composite/TComposite.hh"
#include "composite/CompositeType.hh"
#include "TypeCacher.hh"
#include "scalar/ScalarSetType.hh"
#include "scalar/CircularSetType.hh"
#include "etf/ETFType.hh"
#include "gal/GALType.hh"

#include "MemoryManager.h"
#include "util/dotExporter.h"

#include <iostream>
#include <sstream>
#include <algorithm>

using std::find;


//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


namespace its {

ITSModel::~ITSModel () {
  int i = 0;
  for (types_it it = types_.begin();
       it != types_.end() ;
       ++it, ++i )
    if ( dontdelete.find(i) == dontdelete.end() )
      delete *it;

  if (model_) delete model_;
}

pType ITSModel::findType (Label tname) const {
  for (types_it it = types_.begin();
       it != types_.end() ;
       ++it ) {
    if ((*it)->getName() == tname) {
      return *it;
    }
  }
  return NULL;
}

bool ITSModel::addType (pType  type) {
  pType oldtype = findType (type->getName());
  if (oldtype != NULL) {
    std::cerr << " ITS type " << type->getName() << " already exists in model.\n";
    delete type;
    return false;
  }
  // Add caching mechanism / uncast const qualifier due to pType
  types_.push_back(new TypeCacher(const_cast<Type*>(type)));
  return true;
}

bool ITSModel::setInstance (Label type, Label name) {
  pType ptype = findType (type);
  if (ptype == NULL) {
    std::cerr << "Unknown type " << type << " when treating ITSModel::setInstance." << std::endl;
    return false;
  } else {
    if (model_ != NULL)
      delete model_;
    model_ = new Instance (name, ptype);
    return true;
  }
}

pInstance  ITSModel::getInstance () const {
  return model_;
}

State ITSModel::getInitialState () const {
  return getInstance()->getType()->getState(initName_);
}

bool ITSModel::setInstanceState (Label stateName) {
  if (model_ == NULL)
    return false;
  labels_t inits = getInstance()->getType()->getInitStates () ;
  if ( find(inits.begin(),inits.end(), stateName) != inits.end()) {
    initName_ = stateName;
    return true;
  }
  return false;
}


  Transition ITSModel::getElapse () const {
    /** handle the possibility of timed models with an elapse to make private at top level instance level */
    labels_t totest = getInstance()->getType()->getTransLabels();
    if ( find(totest.begin(),totest.end(),"elapse") != totest.end() ) {
      labels_t tau;
      tau.push_back("elapse");
      Transition elapse = getInstance()->getType()->getSuccs(tau);
      // Huh, no time constraints ?? Forget about time then.
      return elapse;
    } else { 
      return Transition::id;
    }
  }

  // returns the "Next" relation, i.e. exactly one step of the transition relation.
  // tests for presence of "elapse" transition.
  Transition ITSModel::getNextRel () const {
    Transition trans = getInstance()->getType()->getLocals();

    Transition elapse = getElapse(); 
    if (elapse != Transition::id) {
      return trans + elapse;
//      return trans + fixpoint(elapse + Transition::id);
    }
    return trans;
  }


SDD ITSModel::computeReachable (bool wGarbage) const {
  if (reached_ == SDD::null) {
    State M0 = getInitialState ();
    Transition trans = getNextRel();
    
    // top-level = true for garbage collection
    Transition transrel = fixpoint(trans+GShom::id,wGarbage);


    /** This block of commented code implements the "new" states fixpoint loop */
//     std::cout << transrel << std::endl;
//     State M1 = M0;
//     State M2 = M1;
//     while (M2 != State::null) {
//       std::cout << "Step :" << std::endl;
//       getInstance()->getType()->printState(M2,std::cout);
//       State news = trans(M2);
//       M2 = news - M1;
//       M1 = M1 + news;
//     }
    

    reached_ = transrel (M0);

//     if ( GSDD::statistics() <= 100000 ) {
//       exportUniqueTable(reached_, getInstance()->getType()->getName());
//     }

    MemoryManager::garbage();
  }
  return reached_ ;
}

its::Transition ITSModel::getPredRel (State reach_envelope) const
{

  if (predRel_ == Transition::null || reach_envelope != State::null) {
      State reach;
      if (reach_envelope==State::null) {
	reach = computeReachable();
      } else {
	reach = reach_envelope;
      }
      Transition rel = getNextRel().invert(reach);
      bool isExact = ( rel(reach) - reach == State::null );
      if (isExact) {
	predRel_ = rel;
	std::cerr << "Reverse transition relation is exact ! Faster fixpoint algorithm enabled. \n" ;
      } else {
	predRel_ = rel * reach;
	std::cerr << "Reverse transition relation is NOT exact ! Intersection with reachable at each step enabled. \n" ;
      }
      if (reach_envelope != State::null) {
	// Don't cache !
	Transition toret = predRel_;
	predRel_=Transition::null;
	return toret;
      }
    }
    return predRel_;
}

  void ITSModel::printPaths (State init, State toreach, State reach, int limit) const {
    
    typedef std::list<State> rev_t;
    typedef rev_t::iterator rev_it;
    typedef rev_t::reverse_iterator rev_rit;
    rev_t revcomponents;
    
    
    {
      State inter = init * toreach;
      if (inter != State::null) {
	// minimal path is length 0 !
	return ;
      } 
      
    }
    
    State M2,M3;
    M3 = toreach;
    State seen;
    seen = M3;

    revcomponents.push_front(toreach);
    // Reverse construct path to init from toreach
    Transition revTrans = getPredRel(reach);
    //    int step =0;
    while (true) {
      M2 = State::null;
    
      // Remove states from previous step, they cannot be nearer to the goal.
      M2 = revTrans (M3) - seen;
      seen = seen + M2;
      // USEFUL DEBUG TRACES
//       std::cerr << "step " << step++ <<  "  nbstates " << M2.nbStates() <<  endl;
//       getInstance()->getType()->printState(M2,std::cerr);

//      for (vector<Shom>::const_iterator it = reverseRelation.begin(); it != reverseRelation.end(); it++) {
//        M2 = M2 + ( ((*it) (M3))  * ss );
//      }

    // should not happen if the states searched for are rechable by the transition relation
      if (M2 == State::null) {
	std::cerr << "Unexpected empty predecessor set for step : " <<std::endl;
	getInstance()->getType()->printState(M3,std::cerr);
	std::cerr << "returning empty witness path."<< std::endl;
	std::cerr << "Was working with reverse transition :\n" << revTrans << endl;
	std::cerr << "Was working with forward transition :\n" << getNextRel() << endl;
	return ; 
      }//assert(M2 != GSDD::null);

    if (init * M2 == GSDD::null) {
      revcomponents.push_front(M2);
//       cerr << "Backward steps : "<<  revcomponents.size() << endl ;
//       cerr << "Current step-set size : "<< M2.nbStates() << endl;
//       MemoryManager::garbage();
      if (M3 == M2) {
	cerr << "Error detected while attempting to build trace. Your model admits identity as single enabled transition, which should not happen since the state is reachable by forward transition." << endl;
	std::cerr << "Returning empty witness path."<< std::endl;
	std::cerr << "Was working with reverse transition :\n" << revTrans << endl;
	std::cerr << "Was working with forward transition :\n" << getNextRel() << endl;
	return ; 
      }
      M3 = M2;
    } else {
      break;
    }
  } 

    cerr << "Length of minimal path(s) :" << revcomponents.size() <<endl;

    // Forward construction of witness
    
    Type::namedTrs_t namedTrs;
    getInstance()->getType()->getNamedLocals(namedTrs);
    
    /** add elapse if necessary */
    Transition elapse = getElapse(); 
    if (elapse != Transition::id) {
      namedTrs.push_front ( Type::namedTr_t("elapse",elapse));
    }


    std::vector<int>  iters;
    for (int i=0; i < revcomponents.size(); i++) {
      iters.push_back(0);
    }
//    iters[0]=10;
    for (int nbwitness=0 ; nbwitness < limit ; ) {
      State Mi = init;
      State Mi_next = State::null;
      labels_t witness;
      int cur = 0;
      bool ok = false;
      for ( rev_it comp= revcomponents.begin();comp != revcomponents.end();  /*NOP, in loop */) {
	ok = false;
	for ( ; iters[cur] != namedTrs.size() ; ++iters[cur] ) {
	  Type::namedTrs_t::const_iterator it = namedTrs.begin();
	    std::advance(it,iters[cur]);
	    const Type::namedTr_t & trans = *it;

	    Mi_next = ((trans.second) (Mi)) * (*comp) ;

	    if ( Mi_next != State::null) {
	      // transition matches 
	      // 	  std::cout << " Using : " << it->first << endl; 
	      // 	  std::cout << "Reached :" ;
	      // 	  getInstance()->getType()->printState(Mi_next, std::cout);
	      witness.push_back(trans.first);

	      // omg, don't do that !
	      // *comp = Mi_next;

	      Mi = Mi_next;
	      // 	cerr << "ForwardSteps : " <<witness.size() <<endl;
	      // 	MemoryManager::garbage();
	      ok = true;
//	      std::cerr << "match " << trans.first << " at step " << cur << std::endl;
	      break;
	    } else {
//	      std::cerr << "nomatch " << trans.first << " at step " << cur << std::endl;
	    }
	}

	if (! ok) {
	  break;
	} else {
	  ++comp;
	  ++cur;
	}
      }

//      std::cout << "witness " << nbwitness <<  ": "  << std::endl;
      if (ok) {
	// move last cursor
	++iters[cur-1];

	printPath(path_t(witness,init,toreach), std::cout);
	nbwitness++;
	ok = false;
      } else {
	if (cur == 0) {
	  std::cout << "All " << nbwitness << " minimal witness paths have been reported." << std::endl;
	  return;
	} else {
//	  std::cout << "Trying next trans at step " << cur-1 << " of path " <<  std::endl;
	  
	  for (int j = cur; j < revcomponents.size() ; ++j) {
	    iters[j]=0;
	  }
	  cur--;
	  iters[cur]++;	 
	}
      }
	
   }
    
  }

  path_t ITSModel::findPath (State init, State toreach, State reach, bool precise) const {
    
    typedef std::list<State> rev_t;
    typedef rev_t::iterator rev_it;
    typedef rev_t::reverse_iterator rev_rit;
    rev_t revcomponents;

    labels_t witness;

    {
      State inter = init * toreach;
      if (inter != State::null) {
	// minimal path is length 0 !
	return path_t(witness,inter,inter);
      } 

    }

    State M2,M3;
    M3 = toreach;
    State seen;
    seen = M3;

    revcomponents.push_front(toreach);
    // Reverse construct path to init from toreach
    Transition revTrans = getPredRel(reach);
//    int step =0;
    while (true) {
      M2 = State::null;
    
      // Remove states from previous step, they cannot be nearer to the goal.
      M2 = revTrans (M3) - seen;
      seen = seen + M2;
      // USEFUL DEBUG TRACES
//       std::cerr << "step " << step++ <<  "  nbstates " << M2.nbStates() <<  endl;
//       getInstance()->getType()->printState(M2,std::cerr);

//      for (vector<Shom>::const_iterator it = reverseRelation.begin(); it != reverseRelation.end(); it++) {
//        M2 = M2 + ( ((*it) (M3))  * ss );
//      }

    // should not happen if the states searched for are rechable by the transition relation
      if (M2 == State::null) {
	std::cerr << "Unexpected empty predecessor set for step : " <<std::endl;
	getInstance()->getType()->printState(M3,std::cerr);
	std::cerr << "returning empty witness path."<< std::endl;
	std::cerr << "Was working with reverse transition :\n" << revTrans << endl;
	std::cerr << "Was working with forward transition :\n" << getNextRel() << endl;
	return path_t(witness,init,toreach) ;
      }//assert(M2 != GSDD::null);

    if (init * M2 == GSDD::null) {
      revcomponents.push_front(M2);
//       cerr << "Backward steps : "<<  revcomponents.size() << endl ;
//       cerr << "Current step-set size : "<< M2.nbStates() << endl;
//       MemoryManager::garbage();
      if (M3 == M2) {
	cerr << "Error detected while attempting to build trace. Your model admits identity as single enabled transition, which should not happen since the state is reachable by forward transition." << endl;
	std::cerr << "Returning empty witness path."<< std::endl;
	std::cerr << "Was working with reverse transition :\n" << revTrans << endl;
	std::cerr << "Was working with forward transition :\n" << getNextRel() << endl;
	return path_t(witness, init, toreach);
      }
      M3 = M2;
    } else {
      break;
    }
  } 

//    cerr << "Length of minimal path(s) :" << revcomponents.size() <<endl;

    // Forward construction of witness
    
    State Mi = init;
    State Mi_next;
    Type::namedTrs_t namedTrs;
    getInstance()->getType()->getNamedLocals(namedTrs);
    
    /** add elapse if necessary */
    Transition elapse = getElapse(); 
    if (elapse != Transition::id) {
      namedTrs.push_front ( Type::namedTr_t("elapse",elapse));
    }


    for ( rev_it comp= revcomponents.begin();comp != revcomponents.end(); ++comp) {
      bool ok = false;
      for (Type::namedTrs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
	Mi_next = ((it->second) (Mi)) * (*comp) ;
	if ( Mi_next != State::null) {
	  // transition matches 
// 	  std::cout << " Using : " << it->first << endl; 
// 	  std::cout << "Reached :" ;
// 	  getInstance()->getType()->printState(Mi_next, std::cout);
	  witness.push_back(it->first);
	  *comp = Mi_next;
	  Mi = Mi_next;
	  // 	cerr << "ForwardSteps : " <<witness.size() <<endl;
	  // 	MemoryManager::garbage();
	  ok = true;
	  break;
	}
      }
      if (! ok) {
	std::cout << "No transition found to progress in witness path construction. Something is wrong with the transition relation extraction." <<endl;
	std::cout << "Was trying transitions :" <<endl;
	for (Type::namedTrs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
	  std::cout << it->first << " ,";
	}
	std::cout <<endl;
      }
    }

    if (!precise) {
      return path_t(witness,init,toreach);
    }

    // one more pass, executing witness backward
    if (! revcomponents.empty()) {
      State M_i = *revcomponents.rbegin();
      labels_t::const_reverse_iterator wit = witness.rbegin();
      for (rev_rit rit= revcomponents.rbegin(); rit != revcomponents.rend() ; ++rit, ++wit) {
	*rit = M_i * (*rit);
	M_i = *rit;
	Transition revt ;
	for (Type::namedTrs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
	  if (it->first == * wit) {
	    revt = it->second.invert(reach);
	    break;
	  }
	}
	if (revt == Transition::id) {
	  std::cerr << "Unexpectedly did not find transition of witness when performing back-step phase of witness construction." << std::endl;
	  assert(false);
	}
	M_i = revt ( M_i ) ; 
      }
    }
    


//   cerr << "Witness path :" << endl;
//   for (vector<int>::iterator it = witness.begin(); it != witness.end() ; it++) {
//     cerr << "t_" <<*it << "   " ;
//   }
//   cerr << endl;
    if (!witness.empty()) {
      return path_t(witness, *revcomponents.begin(), *revcomponents.rbegin());
    } else {
      return path_t(witness, init, toreach);
    }
}


void ITSModel::print (std::ostream & os) const  {
  for (types_it it = types_.begin();
       it != types_.end();
       ++it) {
    (*it)->print(os);
    os << "\n";
  }
  
  if (model_ != NULL) {
    os << "// target model : \n";
    model_->print(os) ;
    os << " = " << initName_;
    os << " ;" << std::endl;
    model_->getType()->printState(getInitialState(),os);
  }
}

  // Factory role
   // Play factory role for building ITS types from other formalisms
  // Returns false and aborts if type name already exists.
  // Create a type to hold a Petri net.
  bool ITSModel::declareType (const class PNet & net) {
    if (storage_ == sdd_storage)
      return addType(new PNetSType(net));
    else 
      return addType(new PNetType(net));
  }


  // Create a type to hold a Petri net.
  bool ITSModel::declareType (const class TPNet & net) {
    if (storage_ == sdd_storage)
      return addType(new TPNetSType(net));
    else 
      return addType(new TPNetType(net));
  }
  // Create a type to hold a composite
  bool ITSModel::declareType (const class Composite & comp) {
    pType newtype = new CompositeType(comp);
    return addType(newtype);
  }

  bool ITSModel::declareType (const class TComposite & comp) {
    pType newtype = new CompositeType(comp);
    return addType(newtype);
  }
  // Create a type to hold a Scalar set.
  bool ITSModel::declareType (const class ScalarSet & net) {
    ScalarSetType* newtype = new ScalarSetType(net);
    newtype->setStrategy(scalarStrat_, scalarParam_);
    return addType(newtype);
  }

  // Create a type to hold a circular set.
  bool ITSModel::declareType (const class CircularSet & net) {
    CircularSetType* newtype = new CircularSetType(net);
    newtype->setStrategy(scalarStrat_, scalarParam_);
    return addType(newtype);
  }

  // Create a type to hold a circular set.
  bool ITSModel::declareETFType (Label path) {
    EtfType* newtype = new EtfType(path);
    return addType(newtype);
  }

  // Create a type to hold a GAL model
  bool ITSModel::declareType (const class GAL & net) {
    GALType * newtype = new GALType(&net);
    newtype->setStutterOnDeadlock (stutterOnDeadlock_);
    return addType(newtype);
  }



  bool ITSModel::loadOrder (std::istream & is) {
    while (!is.eof()) {
      vLabel line;
      getline(is,line);
      std::stringstream il (line);
      vLabel token;
      il >> token;
      if (token == "#TYPE") {
	vLabel type;
	il >> type;
	labels_t order;
	// get rid of trailing newline (chomp !)
	while (!is.eof()) {
	  vLabel variable;
	  getline(is,variable);
	  if (variable == "#END")
	    break;
	  order.push_back(variable);
	}
// DEBUG
// 	for (labels_it iit = order.begin() ; iit != order.end() ; ++iit)
// 	  std::cout << *iit << std::endl;
	if (!updateVarOrder(type,order))
	  return false;
      } else if (line.empty()) {
	continue;
      } else {
	
	std::cerr << "syntax error in input order file, at line :\n" << line << "\n expected a #TYPE marker\n";
	return false;
      }
    }
    setInstanceState(initName_);
    return true;
  }

  bool ITSModel::updateVarOrder (Label type, const labels_t & order) {
    pType ptype = findType (type);
    if (ptype == NULL) {
      std::cerr << "No such type \"" << type << "\" found in updatevarOrder. "<< std::endl;
      return false;
    } else {
      return ptype->setVarOrder(order);
    }
  }

  void ITSModel::printVarOrder (std::ostream & os) const {
    for (types_it it = types_.begin();
       it != types_.end() ;
       ++it ) {
      os << "#TYPE " << (*it)->getName() << "\n";
      (*it)->printVarOrder(os);
      os << "#END" << std::endl ;
    }
  }
  /** Prints a set of states to a string. The printing invokes the main instance's type's printing mechanism.
   ** The limit is used to avoid excessive sizes of output : only the first "limit" states (or an approximation thereof in SDD context) are shown. **/
  void ITSModel::printSomeStates (State states, std::ostream & out, size_t limit) const {
    if (states.nbStates() <= limit) {
      getInstance()->getType()->printState(states, out);
    } else {
      // (TODO pretty print a single state)" ;
      // we need to select one (or preferably a configurable number of states)
      // a DFS traversal of the is indicated, so this should be part of printState ITS type interface
      // i.e. print states up to a limit.
      out << "[ " << states.nbStates() << " states ]" ;
    }
  }
  
  /** Prints a path. The printing invokes the main instance's type's printing mechanism.
   ** The limit is used to avoid excessive sizes of output : only the first "limit" states (or an approximation thereof in SDD context) are shown. 
   ** The boolean "withStates" controls if only transitions are shown or states as well 
   **/
  void ITSModel::printPath (const path_t &path, std::ostream & out, bool withStates) const {
    if (withStates) {
      out << "From initial states :\n" ;
      printSomeStates(path.getInit(),out);
    }
    out << "This shortest transition sequence of length " << path.getPath().size() << " :\n";
    labels_it end = path.getPath().end();
    for (labels_it it=path.getPath().begin(); it != end ; /*in loop*/) {
      out << *it;
      if (++it != end) {
	out << ", " ;
      }
    }
    out << std::endl;
    if (withStates) {
      out << "Leads to final states :\n" ;
      printSomeStates(path.getFinal(),out);
    }
  }
 

  // Visitor pattern to work on the underlying types
  void ITSModel::visitTypes (class TypeVisitor * visitor) const {
    for (types_it it = types_.begin(); it != types_.end() ; ++it )
      (*it)->visit(visitor);
  }



} // namespace

/** pretty print */
std::ostream & operator << (std::ostream & os, const its::ITSModel & m) {
  m.print(os);
  return os;
}


std::ostream & operator<< (std::ostream & os, const its::Type & t) { return t.print(os) ; }
