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

  // returns the "Next" relation, i.e. exactly one step of the transition relation.
  // tests for presence of "elapse" transition.
  Transition ITSModel::getNextRel () const {
    Shom trans = getInstance()->getType()->getLocals();

    /** handle the possibility of timed models with an elapse to make private at top level instance level */
    labels_t totest = getInstance()->getType()->getTransLabels();
    if ( find(totest.begin(),totest.end(),"elapse") != totest.end() ) {
      labels_t tau;
      tau.push_back("elapse");
      Shom elapse = getInstance()->getType()->getSuccs(tau);
      // Huh, no time constraints ?? Forget about time then.
      if ( elapse != Transition::id )
	trans = trans + elapse;
    }
    return trans;
  }


SDD ITSModel::computeReachable (bool wGarbage) const {
  if (reached_ == SDD::null) {
    State M0 = getInitialState ();
    Transition trans = getNextRel();
    
    // top-level = true for garbage collection
    Transition transrel = fixpoint(trans+GShom::id,wGarbage);
//     std::cout << transrel << std::endl;

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
	std::cout << "Reverse transition relation is exact ! Faster fixpoint algorithm enabled. \n" ;
      } else {
	predRel_ = rel * reach;
	std::cout << "Reverse transition relation is NOT exact ! Intersection with reachable at each step enabled. \n" ;
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

  labels_t ITSModel::findPath (State init, State toreach, State reach) const {
    
    typedef std::list<State> rev_t;
    typedef rev_t::const_iterator rev_it;
    rev_t revcomponents;

    labels_t witness;
    State M2,M3;
    M3 = toreach;

    revcomponents.push_front(toreach);
    // Reverse construct path to init from toreach
    Transition revTrans = getPredRel(reach);
    while (true) {
      M2 = State::null;
    
      M2 = revTrans (M3);
//      for (vector<Shom>::const_iterator it = reverseRelation.begin(); it != reverseRelation.end(); it++) {
//        M2 = M2 + ( ((*it) (M3))  * ss );
//      }

    // should not happen if the states searched for are rechable by the transition relation
      if (M2 == State::null) {
	std::cerr << "Unexpected empty predecessor set for step : " <<std::endl;
	getInstance()->getType()->printState(M3,std::cerr);
	std::cerr << "returning empty witness path."<< std::endl;
	return witness;
      }//assert(M2 != GSDD::null);

    if (init * M2 == GSDD::null) {
      revcomponents.push_front(M2);
//       cerr << "Backward steps : "<<  revcomponents.size() << endl ;
//       cerr << "Current step-set size : "<< M2.nbStates() << endl;
//       MemoryManager::garbage();
      M3 = M2;
    } else {
      break;
    }
  } 
    cerr << "Length of minimal path(s) :" << revcomponents.size() <<endl;

  // Forward construction of witness

    State Mi = init;
    State Mi_next;
    Type::namedTrs_t namedTrs;
    getInstance()->getType()->getNamedLocals(namedTrs);

    for ( rev_it comp= revcomponents.begin();comp != revcomponents.end(); comp++) {
      for (Type::named_Trs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
	Mi_next = ((it->second) (Mi)) * (*comp) ;
	if ( Mi_next != State::null) {
	  // transition matches 
	  witness.push_back(it->first);
	  Mi = Mi_next;
	  // 	cerr << "ForwardSteps : " <<witness.size() <<endl;
	  // 	MemoryManager::garbage();
	  break;
	}
      }
    }

//   cerr << "Witness path :" << endl;
//   for (vector<int>::iterator it = witness.begin(); it != witness.end() ; it++) {
//     cerr << "t_" <<*it << "   " ;
//   }
//   cerr << endl;

  return witness;

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
