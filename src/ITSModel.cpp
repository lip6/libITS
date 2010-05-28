#include "ITSModel.hh"

// For factory role
#include "PNetType.hh"
#include "JSONPNetType.hh"
#include "TPNetType.hh"
#include "Composite.hh"
#include "TComposite.hh"
#include "CompositeType.hh"
#include "TypeCacher.hh"
#include "ScalarSetType.hh"
#include "CircularSetType.hh"
#include "MemoryManager.h"
//#include "util/dotExporter.h"

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
  // Create a type to hold a Petri net, with hierarchical representation based on JSON description.
  bool ITSModel::declareType (const class PNet & net, const json::Hierarchie * hier) {
    return addType(new JsonPNetType(net,hier));
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
	getline(is,line);
	while (!is.eof()) {
	  vLabel variable;
	  getline(is,variable);
	  if (variable == "#END")
	    break;
	  order.push_back(variable);
	}
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
