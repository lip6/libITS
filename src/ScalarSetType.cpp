
#include <iostream>
#include "ScalarSetType.hh"
#include "Composite.hh"

namespace its {

/** the set InitStates of designated initial states (a copy)*/
labels_t ScalarSetType::getInitStates () const {
  labels_t ret;
  for (ScalarSet::cstates_it it = comp_.cstates_begin(); it != comp_.cstates_end() ; ++it) {
    ret.push_back(it->first);
  }
  return ret;
}

/** the set T of public transition labels (a copy)*/
labels_t ScalarSetType::getTransLabels () const {
  std::set<vLabel> toRet;
  for (ScalarSet::syncs_it it = comp_.syncs_begin(); it != comp_.syncs_end() ; ++it) {
      toRet.insert(it->getLabel());
  }
  labels_t ret;
  for (std::set<vLabel>::const_iterator it = toRet.begin() ; it != toRet.end() ; ++it ) {
    ret.push_back(*it);
  }
  return ret;
}


  //////// strategies
  // Strategies
  class BasicStrategy : public RepresentationStrategy {
  protected :
    const ScalarSet & comp_;
    ITSModel & model_;
    int grain_;

    vLabel instanceName (int i) const { return comp_.getInstance().getName() + "_" + to_string(i) ; }

    vLabel typeName (int i) const {
      if (i==1)
	return comp_.getInstance().getType()->getName() ;
      else
	return comp_.getInstance().getType()->getName() + "_" + to_string(i) ;
    }
  public :
    BasicStrategy (const ScalarSet & comp, ITSModel & m, int param): comp_(comp),model_(m),grain_(param) { 
      // protect against bad value, causes a crash on (n % grain) otherwise
      if (grain_ == 0) 
	grain_ =1;
    };

    // build a representation of N = n*grain + complement instances
    // with grain > complement. Normally used with N/grain = n, N % grain = complement.
    // encoding uses $complement$ blocks of $grain+1$ instances
    // and n-complement blocks of n
    void buildNaryRepresentation (int n , int grain=1, int complement =0){
      int N = n*grain + complement;
      
      Composite net (typeName(N));
      
      for (int i=0 ; i < complement ; ++i) {
	net.addInstance(instanceName(i),typeName(grain+1),model_);
      }
      for (int i=complement; i< n; ++i) {
	net.addInstance(instanceName(i),typeName(grain),model_);
      }
      // delegators
      for (ScalarSet::syncs_it it = comp_.syncs_begin() ; it != comp_.syncs_end() ; ++it ){
	Label sname = it->getName();
	Label slabel = it->getLabel();
	if ( it->isALL() ) {
	  // handle it as a single sync with $n$ parts
	  net.addSynchronization( sname, slabel );
	  for (int i=0 ; i < n ; ++i) {
	    net.addSyncPart (sname , instanceName(i), sname);
	  }
	} else {
	  // handle it as $n$ syncs each with a single part
	  for (int i=0 ; i < n ; ++i) {
	    vLabel newName = sname + to_string(i); 
	    net.addSynchronization( newName , slabel );
	    net.addSyncPart (newName , instanceName(i), sname);
	  }
	}
      }

      // initial state
      for (ScalarSet::cstates_it it = comp_.cstates_begin() ; it != comp_.cstates_end() ; ++it ){
	Label stname = it->first;
	// instance number
	size_t i = 0;
	size_t ndefault = it->second.getDefaultStateCard( comp_.size() );
	for (  ; i < ndefault ; ++i) { 	  
	  net.updateStateDef (stname , instanceName(i), it->second.getDefault() );
	}
	for (ScalarState::assignments_it assit = it->second.begin() ; assit != it->second.end() ; ++assit ) {
	  for (size_t j=0 ; j < assit->second && i < comp_.size() ; ++j, ++i ) {
	    net.updateStateDef (stname , instanceName(i), assit->first );
	  }
	}
	assert ( i ==  comp_.size());
      }
      model_.declareType(net);
      
    }

    virtual vLabel buildRepresentation ( ) {
      // clone the basic contained type
      model_.cloneType(comp_.getInstance().getType());
      int n = comp_.size();
      if (n == 1) {
	// avoid building groups of size 1...
	return typeName(1);
      }
      if (grain_ > n/2)
	// limit strange borderline values
	grain_ = n/2 >0 ? n/2 : 1;
      if (grain_ > 1)
	// default parameters make this a simple composition of grain instances of the contained type of the scalar (e.g. PNet)
	buildNaryRepresentation ( grain_ );
      if (  n % grain_ != 0)
	// we have a non empty complement
	// default parameters again make this a simple 1 depth composition
	buildNaryRepresentation ( grain_ + 1 );
 
      buildNaryRepresentation (n/grain_, grain_, n % grain_) ;
      return typeName(n);
    } 
  };

  class DepthRecStrategy : public BasicStrategy {
    double factor_;
  public :
    DepthRecStrategy (const ScalarSet & comp, ITSModel & m, int param = 1, double factor = 1.0): BasicStrategy(comp,m,param), factor_(factor) {};
    

    void buildNWiseComposite (int n, int grain)  {
      int part = n / grain;
      int rem = n % grain ;
      if (model_.findType(typeName(n)) != NULL)
	return ;
      else if (n < grain || grain == 1) {
	// use grain 1 for final level
	buildNaryRepresentation (n);
	return ;
      } else if (part > 1)
	buildNWiseComposite (part, (int) (grain * factor_) );

      if (rem != 0)
	buildNWiseComposite (part + 1, (int) (grain * factor_) );

//       std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " blocks " 
// 		<< " of sizes " << (grain-1) << "*" << part << ".." << part + rem << std :: endl ;
      buildNaryRepresentation(grain,part,rem);
    }

    virtual vLabel buildRepresentation ( ) {
     // clone the basic contained type
      model_.cloneType(comp_.getInstance().getType());
      size_t n = comp_.size();
      buildNWiseComposite(n, grain_);
      return typeName(n);
    }

  };


  class ShallowRecStrategy : public BasicStrategy {
  public :
    ShallowRecStrategy (const ScalarSet & comp, ITSModel & m, int param = 1): BasicStrategy(comp,m,param) {};
    

    void buildNWiseComposite (int n, int grain)  {
      if (model_.findType(typeName(n)) != NULL)
	return ;
      else if (n < grain || grain == 1) {
	// use grain 1 for final level
	buildNaryRepresentation (n);
	return ;
      }

      int nblocks = n;
      int rem = 0;
      while (true) {
	int tmp = nblocks / grain;
	if (tmp <= grain)
	  break;
	rem = nblocks % grain ;
	nblocks = tmp;
      }
      
      int blocksize = n / nblocks;

      if (rem != 0)
	buildNWiseComposite (blocksize + 1, grain  );
      buildNWiseComposite (blocksize, grain );


//       std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " in " << nblocks << " blocks " 
// 		<< " of sizes " << (part+1) << "*" << rem << "+" << part << "*" << nblocks - rem << std :: endl ;
      buildNaryRepresentation(nblocks,blocksize,rem);
    }

    virtual vLabel buildRepresentation ( ) {
     // clone the basic contained type
      model_.cloneType(comp_.getInstance().getType());
      size_t n = comp_.size();
      buildNWiseComposite(n, grain_);
      return typeName(n);
    }

  };


  void ScalarSetType::setStrategy (scalarStrategy strat, int parameter) {
    if (strat_ != NULL) {
      model_ = ITSModel();
      concrete_ = NULL;
      delete strat_;
      strat_ = NULL;
    }
    switch (strat) {
    case DEPTH1 :
      strat_ = new BasicStrategy(comp_, model_, parameter);
      break ;
    case DEPTHREC :
      strat_ = new DepthRecStrategy(comp_,model_,parameter);
      break;
    case SHALLOWREC :
      strat_ = new DepthRecStrategy(comp_,model_,parameter);
      break;
    default :
      strat_ = new BasicStrategy(comp_, model_, parameter);
      break;
    }
    return;
  }

  pType ScalarSetType::getConcrete () const {
    if (concrete_ == NULL ) {
      vLabel name = strat_->buildRepresentation();
      concrete_ =  model_.findType(name);
    }
    return concrete_;
  }


} // namespace

