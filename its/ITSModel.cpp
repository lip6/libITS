#include "its/ITSModel.hh"

// For factory role
#include "its/petri/PNetType.hh"
#include "its/petri/TPNetType.hh"
#include "its/composite/Composite.hh"
#include "its/composite/TComposite.hh"
#include "its/composite/CompositeType.hh"
#include "its/TypeCacher.hh"
#include "its/scalar/ScalarSetType.hh"
#include "its/scalar/CircularSetType.hh"
#include "its/etf/ETFType.hh"
#include "its/gal/GALType.hh"
#include "its/divine/dve2GAL.hh"

#include "ddd/MemoryManager.h"
#include "ddd/util/dotExporter.h"

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

  void  ITSModel::getNamedLocals (Type::namedTrs_t & namedTrs) const{
    getInstance()->getType()->getNamedLocals(namedTrs);
    Transition elapse = getElapse(); 
    if (elapse != Transition::id) {
      namedTrs.push_front ( Type::namedTr_t("elapse",elapse));
    }
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
      Transition nextRel = getNextRel();
      if (nextRel == Transition::null) {
	return Transition::null;
      }
      Transition rel = nextRel.invert(reach);
// 	std::cerr << "Was working with reverse transition :\n" << rel << endl;
// 	std::cerr << "Was working with forward transition :\n" << getNextRel() << endl;
      State border = rel(reach) - reach;
      bool isExact = ( border == State::null );
      if (isExact) {
	predRel_ = rel;
	std::cerr << "Reverse transition relation is exact ! Faster fixpoint algorithm enabled. \n" ;
      } else {
	d3::set<GShom>::type toadd;
	d3::set<GShom>::type toprotect;
	Type::namedTrs_t namedTrs ;
	getNamedLocals(namedTrs);

	Type::namedTrs_t::const_iterator  transit;
	stringstream translist ;
	for (transit = namedTrs.begin() ; transit != namedTrs.end() ; ++transit) {
	  State res = transit->second (reach );
	  if (res != State::null) {
	    State preds = transit->second.invert(reach) (reach);
	    State falsepreds =  preds - reach;
	    if ( falsepreds != State::null) {
	      translist << transit->first << ", " ;
	      // std::cerr << "Transition reverse adds behavior : "<< transit->first << std::endl;
	      // printSomeStates(falsepreds,std::cout,20);
	      // std::cerr << "Some legit states : ";
	      // printSomeStates(preds * reach,std::cout,20);
	      toprotect.insert(transit->second);
	    } else {
	      toadd.insert(transit->second);
	    }
	  } 
	  // else {
	  //	    std::cerr << "Transition is unreachable killing from backward relation : "<< transit->first << std::endl;
	  // }
	}
	// if (toadd.size() > 0 || toprotect.size() < namedTrs.size()) {
	//   std::cout << "Hit reverse partial " << toprotect.size() << "/" <<  toadd.size() << "/" << namedTrs.size() << std::endl;
	// } 
	if (toadd.size()> 0) {
		predRel_ = Transition::add(toadd).invert(reach) + Transition::add(toprotect).invert(reach) * reach;
	} else {
		predRel_ = rel * reach;
	}
	//	predRel_ = rel * reach;
	//predRel_ = (Transition::id - border ) & rel;
	std::cerr << "Reverse transition relation is NOT exact ! Due to transitions " << translist.str() 
		  << " Intersection with reachable at each step enabled. (destroyed/reverse/intersect/total) :" 
		  << (namedTrs.size()-toprotect.size() -toadd.size()) << "/"   <<  toadd.size() << "/" << toprotect.size() << "/" << namedTrs.size() << std::endl;
	//printSomeStates( rel(reach) - reach, std::cout, 400000);
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

  void ITSModel::playPath (labels_t path) const {
    State init = getInitialState();

    // Forward construction of witness
    
    Type::namedTrs_t namedTrs;
    getNamedLocals(namedTrs);
        
    State cur = init;
    int i=0;
    for (labels_it pathit = path.begin() ; pathit != path.end() ; ++pathit) {
      Type::namedTrs_t::const_iterator  transit;
      for (transit = namedTrs.begin() ; transit != namedTrs.end() ; ++transit) {
	if (transit->first == *pathit) {
	  break;
	}
      }
      if (transit == namedTrs.end() ) {
	std::cerr << "ERROR : Unknown transition " << *pathit << " in path. Please check that transition exists." << std::endl;
	return;
      }
      cur = (transit->second) (cur);
      if (cur == State::null) {
	std::cout << "At step " << i << " could not execute transition :" << transit->first <<std::endl;
	std::cout << "Replay trace mode failed : could not execute transition sequence." <<std::endl;
	return;
      }
      ++i;
    }
    std::cout << "Replay trace successfully played your trace. Reached states (10 max shown):" << std::endl;
    printSomeStates(cur,std::cout,printLimit_);
  }

  void ITSModel::printPaths (State init, State toreach, State reach, size_t limit) const {
    
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
    getNamedLocals(namedTrs);

    std::vector<size_t>  iters;
    for (size_t i=0; i < revcomponents.size(); i++) {
      iters.push_back(0);
    }
//    iters[0]=10;
    for (size_t nbwitness=0 ; nbwitness < limit ; ) {
      State Mi = init;
      State Mi_next = State::null;
      labels_t witness;
      size_t cur = 0;
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
	
	if (! printStatesInTrace_) {
	  std::cout << "Imprecise witness reported."<<std::endl ;
	  printPath(path_t(witness,init,toreach), std::cout,true);	
	} else {
	  // show intermediate states
	  // one more pass, executing witness backward
	  if (! revcomponents.empty()) {
	    // copy it
	    rev_t revcopy = revcomponents;
	    labels_t::const_reverse_iterator wit = witness.rbegin();
	    for (rev_rit rit= revcopy.rbegin(); rit != revcopy.rend() ; ++rit, ++wit) {
	      // We look for a transition :  *rit   - t^-1 -> *pred
	      rev_rit pred = rit;
	      ++pred;
	      if (pred == revcopy.rend()) {
		break;
	      }
	      
	      // lookup and build reverse transition
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
	      
	      
	      *pred = (*pred) *  revt( *rit ) ; 
	    }

	    if (!witness.empty()) {
	      printPath(path_t(witness, revcopy.begin(), revcopy.end()), std::cout,true);
	    }
	  }
	}

	nbwitness++;
	ok = false;
      } else {
	if (cur == 0) {
	  std::cout << "All " << nbwitness << " minimal witness paths have been reported." << std::endl;
	  return;
	} else {
//	  std::cout << "Trying next trans at step " << cur-1 << " of path " <<  std::endl;
	  
	  for (size_t j = cur; j < revcomponents.size() ; ++j) {
	    iters[j]=0;
	  }
	  cur--;
	  iters[cur]++;	 
	}
      }
	
   }
    
  }

class ExtractOneState {
private:
  bool firstError;
public : 
  ExtractOneState () : firstError(false) {}
  SDD compute (const GSDD & reach) {
    if (reach==GSDD::null || reach==GSDD::one) {
      return reach;
    }
    auto it = reach.begin();
    const DataSet * ev = it->first;
    // Used to work for referenced DDD
    if (typeid(*ev) == typeid(GSDD) ) {
      return SDD(reach.variable(),  compute ( GSDD ((SDD &) *ev)), compute(it->second));
    } else if (typeid(*ev) == typeid(DDD)) {
      return SDD(reach.variable(),  compute ( GDDD ((DDD &) *ev)), compute(it->second));
      //    } else if (typeid(*g) == typeid(IntDataSet)) {
      // nothing, no nodes for this implem
      //return stat_t();
    } else {
      if (firstError) {
        std::cerr << "Warning : unknown referenced dataset type on arc, node count is inacurate"<<std::endl;
        std::cerr << "Read type :" << typeid(*ev).name() <<std::endl ;
	firstError = false;
      }
      return GSDD::null;
    }

  }
  DDD  compute (const GDDD & reach)  {
    if (reach==GDDD::null || reach==GDDD::one) {
      return reach;
    }
    auto it = reach.begin();
    return DDD(reach.variable(), it->first, compute(it->second));
  }

};




  path_t ITSModel::findCycle (State init, State scc) const {
    ExtractOneState eos;
    State oneinit = eos.compute(init);
    State pred = getPredRel() (oneinit) * scc;

    if (pred == State::null) {
      std::cerr << "Empty predecessor step."<< std::endl;
    }
    path_t path = findPath(oneinit,pred,scc,true);

    // now close the loop
    Type::namedTrs_t trs;
    getNamedLocals(trs);
    vLabel tr;
    State target;
    for (const auto & it : trs ) {
      target = it.second (path.getFinal()) * path.getInit();
      if (target != State::null) {
	tr = it.first;
	* path.getStates().rbegin() = path.getFinal()*  getPredRel() (target);
	break;
      }
    }
    path.getStates().push_back(target);
    path.getPath().push_back(tr);
    
    return path;
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
      revcomponents.push_front(M2 * init);
      break;
    }
  } 

//    cerr << "Length of minimal path(s) :" << revcomponents.size() <<endl;

    // Forward construction of witness
    
    Type::namedTrs_t namedTrs;
    getNamedLocals(namedTrs);
    
    // iterate looking for n-1 transitions
    for ( rev_it cur= revcomponents.begin();cur != revcomponents.end(); ++cur) {
      // we look for appropriate transition from cur to next
      rev_it next = cur;
      next++;
      if (next == revcomponents.end()) {
	break;
      }

      bool ok = false;
      // iterate individual transitions
      for (Type::namedTrs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
	State Mi_next = ((it->second) (*cur)) * (*next) ;
	if ( Mi_next != State::null) {
	  // transition matches 
// 	  std::cout << " Using : " << it->first << endl; 
// 	  std::cout << "Reached :" ;
// 	  getInstance()->getType()->printState(Mi_next, std::cout);

	  witness.push_back(it->first);
	  *next = Mi_next;
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
    assert(! revcomponents.empty()) ;

    if (!precise && ! printStatesInTrace_) {
      std::cout << "Imprecise witness reported."<<std::endl ;
      return path_t(witness,*revcomponents.begin(),*revcomponents.rbegin());
    }

    // one more pass, executing witness backward
    if (! revcomponents.empty()) {
      labels_t::const_reverse_iterator wit = witness.rbegin();
      for (rev_rit rit= revcomponents.rbegin(); rit != revcomponents.rend() ; ++rit, ++wit) {
	// We look for a transition :  *rit   - t^-1 -> *pred
	rev_rit pred = rit;
	++pred;
	if (pred == revcomponents.rend()) {
	  break;
	}

	// lookup and build reverse transition
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


	*pred = (*pred) *  revt( *rit ) ; 
      }
    }

//   cerr << "Witness path :" << endl;
//   for (vector<int>::iterator it = witness.begin(); it != witness.end() ; it++) {
//     cerr << "t_" <<*it << "   " ;
//   }
//   cerr << endl;
    if (!witness.empty()) {
      if ( printStatesInTrace_ ) {
	return path_t(witness, revcomponents.begin(), revcomponents.end());
      } else {
	return path_t(witness, *revcomponents.begin(), *revcomponents.rbegin());
      }
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
    newtype->setOrderStrategy(orderHeuristic_);
    return addType(newtype);
  }

  // Create a type to hold a GAL model
  static const vLabel empty_string = "";
  Label ITSModel::declareDVEType (Label path) {
    struct dve2GAL::dve2GAL * loader = new dve2GAL::dve2GAL ();
    std::string modelName = wibble::str::basename(path);
    vLabel ext (".dve");
    modelName.replace(modelName.find(ext),ext.length(),"");
    std::replace( modelName.begin(), modelName.end(), '-', '_');
    std::replace( modelName.begin(), modelName.end(), '.', '_');
    loader->setModelName(modelName);
    loader->read( path.c_str() );
    loader->analyse();
    GALType * res = new GALDVEType (loader->convertFromDve(), loader);
    res->setStutterOnDeadlock (stutterOnDeadlock_);
    res->setOrderStrategy(orderHeuristic_);
    if (addType(res)) {
      return res->getName();
    } else {
      return empty_string;
    }
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
      printSomeStates(path.getInit(),out,printLimit_);
      out << std::endl;
    }
    size_t sz = path.getPath().size() ;
    if (sz ==0) {
      out << "The initial states satisfy the goal. Shortest path is empty transition sequence. \n";
      return;
    } 
    out << "This shortest transition sequence of length " << sz << " :\n";
    labels_it end = path.getPath().end();
    for (labels_it it=path.getPath().begin(); it != end ; /*in loop*/) {
      out << *it;
      if (++it != end) {
	out << ", " ;
      }
    }
    out << std::endl;

    if (printStatesInTrace_) {
      std::cout << "Precise witness with states :" << std::endl;
      auto wit = path.getPath().begin();
      int i = 0;
      for (auto rit = path.getStates().begin(); rit != path.getStates().end() ; ++rit, ++wit) {
	printSomeStates ( *rit, std::cout) ;
	if (wit != path.getPath().end())
	  std::cout << "Transition " << i++ << " fire : " << *wit << std::endl;  
      }
    }

    out << std::endl;
    if (withStates) {
      out << "Leads to final states :\n" ;
      printSomeStates(path.getFinal(),out,printLimit_);
    }
  }

  class ValueExtractor {
    d3::hash_set<GDDD>::type seend3;
    d3::hash_set<GSDD>::type seen;

  public : 
    const Type::varindex_t & index;
    DDD values;
    bool firsterror;
    ValueExtractor(const Type::varindex_t & index_):index(index_),values(DDD::null), firsterror(true){}

    void visitEdge (const DataSet* g, int curindex)
    {
      // Used to work for referenced DDD
      if (typeid(*g) == typeid(GSDD) ) {
	visit ( GSDD ((SDD &) *g), curindex );
      } else if (typeid(*g) == typeid(DDD)) {
	visit ( GDDD ((DDD &) *g), curindex );
      //    } else if (typeid(*g) == typeid(IntDataSet)) {
      // nothing, no nodes for this implem
      } else {
	if (firsterror) {
	  std::cerr << "Warning : unknown referenced dataset type on arc, node count is inacurate"<<std::endl;
	  std::cerr << "Read type :" << typeid(*g).name() <<std::endl ;
	  firsterror = false;
	}
      }
    }

    void visit (const GSDD & d, int curindex) {
      if ( d == GSDD::one || d == GSDD::null){
	if (firsterror) {
	  std::cerr << "Warning : variable not found when computing bounds."<<std::endl;
	  firsterror = false;
	}
	return;
      }
      // hit ?
      if (seen.find(d) == seen.end()) {
	// add to seen
	seen.insert(d);
	// grab target index
	int target = index[curindex];

	if (d.variable() == target) {
	  // extract
	  for(GSDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
	    visitEdge(gi->first,curindex+1);
	  }
	} else {
	  // navigate
	  for(GSDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
	    visit(gi->second,curindex);
	  }	  
	}
      }
    }

    void visit (const GDDD & d, int curindex) {
      if ( d == GDDD::one || d == GDDD::null){
	if (firsterror) {
	  std::cerr << "Warning : variable not found when computing bounds."<<std::endl;	
	  std::cerr << "Current index : " << curindex;
	  std::cerr << "var index : " ;
	  std::cerr << "[";
	  for (Type::varindex_t::const_iterator it = index.begin() ; it != index.end() ; ++it) {
	    std::cerr <<  *it << "," ;
	  }
	  std::cerr << "]" << std::endl;
	  firsterror = false;
	}
	return;
      }
      // hit ?
      if (seend3.find(d) == seend3.end()) {
	// add to seen
	seend3.insert(d);
	// grab target index
	int target = index[curindex];

	if (d.variable() == target) {	  
	  // extract
	  for(GDDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
	    values = values + GDDD(0,gi->first);
	  }
	} else {
	  // navigate
	  for(GDDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
	    visit(gi->second,curindex);
	  }	  
	}
      }
    }

  };

  DDD extractValues (const Type::varindex_t & index,State s) {
    ValueExtractor ve (index);
    ve.visit(s,0);
    return ve.values;
  }

  /** Get bounds for a variable : the <min,maximum> value the variable can reach in the given state space. */
  std::pair<int,int> ITSModel::getVarRange (Label variable, State states) const {
    Type::varindex_t index;
    getInstance()->getType()->getVarIndex(index,variable);
    
    // std::cout << "var index : " ;
    // std::cout << "[";
    // for (Type::varindex_t::const_iterator it = index.begin() ; it != index.end() ; ++it) {
    //   std::cout <<  *it << "," ;
    // }
    // std::cout << "]" << std::endl;

    DDD d = extractValues(index,states);
    
    if (d == DDD::null) {
      std::cerr << "Error handling bounds for variable " << variable << std::endl;
      return std::make_pair(1,-1);
    }
    
    std::pair<int,int> toret;
    GDDD::const_iterator gi=d.begin();
    if (gi != d.end()) {
      toret.first = gi->first; 
      toret.second = gi->first;
    }
    for(;gi!=d.end();++gi) {
      toret.first = std::min(toret.first,(int)gi->first); 
      toret.second = std::max(toret.second,(int)gi->first); 
    }
    return toret;
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
