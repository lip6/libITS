#include <typeinfo>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "ScalarSetType.hh"
#include "CircularSetType.hh"
#include "composite/Composite.hh"

#define trace while(0) std::cerr

namespace its {



/** the set InitStates of designated initial states (a copy)*/
labels_t ScalarSetType::getInitStates () const {
  labels_t ret;
  for (ScalarSet::cstates_it it = getComp().cstates_begin(); it != getComp().cstates_end() ; ++it) {
    ret.push_back(it->first);
  }
  return ret;
}

/** the set T of public transition labels (a copy)*/
labels_t ScalarSetType::getTransLabels () const {
  std::set<vLabel> toRet;
  for (ScalarSet::syncs_it it = getComp().syncs_begin(); it != getComp().syncs_end() ; ++it) {
      toRet.insert(it->getLabel());
  }
  labels_t ret;
  for (std::set<vLabel>::const_iterator it = toRet.begin() ; it != toRet.end() ; ++it ) {
    ret.push_back(*it);
  }
  return ret;
}
  class InstanceNameFinder : public TypeVisitor {
    int n_;
    vLabel prefix_;
    RepresentationStrategy * strat_;
  public :
    InstanceNameFinder(int n, RepresentationStrategy * strat):n_(n), strat_(strat) {};
    ~InstanceNameFinder() {};
    Label getPrefix() { return prefix_; }
    // or also could be a TPNet
    void visitPNet (const class PNet & net) { /* Terminal recursion case, we are fine, stop here. */ ; }
    void visitComposite (const class Composite & net) { 
      for (Composite::comps_it it = net.comps_begin() ; it != net.comps_end() ; ++ it) {
	int isize = strat_->getTypeSize ( it->getType()->getName() );
	if (n_ < isize) {
	  prefix_ += it->getName() + ".";
	  if (isize == 1)
	    return;
	  it->getType()->visit(this);
	  break;
	} else {
	  n_ -= isize;
	}
      }
    }
    void visitScalar (const class ScalarSet & net) { std::cerr << "Unexpected nested scalar/circular types. Failing, sorry."<< std::endl ; assert(false); }
    void visitCircular (const class CircularSet & net) { std::cerr << "Unexpected nested scalar/circular types. Failing, sorry."<< std::endl ; assert(false); }
  };



    /** The state predicate function : string p -> SHom.
   *  returns a selector homomorphism that selects states verifying the predicate 'p'.
   *  The syntax of the predicate is left to the concrete type realization.
   *  The only constraint is that the character '.' is used as a namespace separator
   *  and should not be used in the concrete predicate syntax.
   *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
  Transition ScalarSetType::getAPredicate (Label predicate) const {
    
    // Step 1 : parse the predicate index up to "."
    const char * pred = predicate.c_str();
    vLabel remain, var;
    for (const char * cp = pred ; *cp ; ++cp) {
      if ( *cp == '.' ) {
	remain = cp+1;
	break;
      } else {
	var += *cp;
      }
    }

    // Step 2 : check if it is an integer
    int value = 0;
    if ( sscanf (var.c_str(), "%d" , &value) == 0 ) {
      std::cerr << "Unable to parse : " << var << " to an integer designating an instance when parsing predicate " << predicate << std::endl;
      std::cerr << "Error is fatal, failing with error code 2" << std::endl;
      exit (2);      
    }
    
    InstanceNameFinder inf (value, strat_);
    // Step 3 : resolve within concrete.
    getConcrete()->visit( & inf );
    vLabel prefix = inf.getPrefix();
    
//     std::cerr << "Found prefix : "<< prefix << std::endl;
//     std::cerr << "When searching instance : "<< value << std::endl;
//     std::cerr << "In model : "<< *getConcrete() << std::endl;

    return getConcrete()->getPredicate(prefix + remain);
  }



/** the set T of public transition labels (a copy)*/
labels_t CircularSetType::getTransLabels () const {
  std::set<vLabel> toRet;
  // cumulate to "super" labels
  labels_t super = ScalarSetType::getTransLabels();
  for (labels_it it = super.begin() ; it != super.end() ; ++it)
    toRet.insert(*it);
  
  // Add circular sync labels
  const CircularSet & comp = getCComp();
  for (CircularSet::circs_it it = comp.circs_begin() ; it != comp.circs_end(); ++it)
    toRet.insert(it->getLabel());

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
    int getTypeSize (Label type) const {
      if (type == comp_.getInstance().getType()->getName())
	return 1 ;
      const char * typestr = type.c_str();
      const char * cp;
      for (cp = typestr + strlen(typestr) - 1 ;
	   *cp != '_';
	   --cp)  /*NOP*/ ;
      // go back beyond the '_'
      ++cp;
      return atoi(cp);
    }
    
    virtual ~BasicStrategy () {}

  protected:

    /** returns a pointer to circularSet if available (i.e. we are actually building a circular set) or NULL otherwise */
    const CircularSet * getCComp () const {
      return dynamic_cast<const CircularSet*> ( &comp_ ) ;
    }
  public :
    BasicStrategy (const ScalarSet & comp, ITSModel & m, int param): comp_(comp),model_(m),grain_(param) { 
      // protect against bad value, causes a crash on (n % grain) otherwise
      if (grain_ == 0) 
	grain_ =1;
    };
    // in a context with n internal instances, connect 0 to 1, 1 to 2 etc.. n-2 to n-1 with private syncs.
    void addInternal (Composite *net, const CircularSet * ccomp, int n) const {
      trace << "Augmenting Nary n=" << n << " composition " << net->getName() << " with internal private transitions." << std::endl ;
      for (CircularSet::circs_it it = ccomp->circs_begin() ; it != ccomp->circs_end(); ++it) {
	vLabel tname;
	for (int i=0; i< n-1; ++i) {
	  int next = i + 1  ;
    
	  tname = it->getName()+to_string(i);
	  // empty label indicates a private event
	  net->addSynchronization(tname,it->getLabel());	  
	  for (labels_it lit = it->getCurrentLabels().begin() ; 
	       lit != it->getCurrentLabels().end() ;
	       ++lit) {
	    net->addSyncPart(tname,instanceName(i),*lit);
	  }
	  for (labels_it lit = it->getNextLabels().begin() ; 
	       lit != it->getNextLabels().end() ;
	       ++lit) {
	    net->addSyncPart(tname,instanceName(next),*lit);
	  }
	}
	
      }
    }
    // in a context with n internal instances, connect n-1 to 0 with private syncs.
    void addLoop (Composite *net, const CircularSet * ccomp, int n) const {
      trace  << "Augmenting Nary n=" << n << " composition " << net->getName() << " with circular private syncs"  << std::endl ;
      for (CircularSet::circs_it it = ccomp->circs_begin() ; it != ccomp->circs_end(); ++it) {
	vLabel tname;
	int i =n-1;
	int next = 0;
	
	tname = it->getName()+to_string(i);
	// empty label indicates a private event
	net->addSynchronization(tname,it->getLabel());	  
	for (labels_it lit = it->getCurrentLabels().begin() ; 
	     lit != it->getCurrentLabels().end() ;
	     ++lit) {
	  net->addSyncPart(tname,instanceName(i),*lit);
	}
	for (labels_it lit = it->getNextLabels().begin() ; 
	     lit != it->getNextLabels().end() ;
	     ++lit) {
	  net->addSyncPart(tname,instanceName(next),*lit);
	}
      }
    }
    // in a context with n internal instances, create public "hooks" on left of 0 and right of n-1.
    void addEdges (Composite *net, const CircularSet * ccomp, int n) const {
      trace  << "Augmenting Nary n=" << n << " composition " << net->getName() << " with public visibility syncs at edges "  << std::endl ;
      for (CircularSet::circs_it it = ccomp->circs_begin() ; it != ccomp->circs_end(); ++it) {
	vLabel tname;
	int i =n-1;
	int next = 0;
	
	tname = it->getName()+to_string(i);
	for (labels_it lit = it->getCurrentLabels().begin() ; 
	     lit != it->getCurrentLabels().end() ;
	     ++lit) {
	  net->addSynchronization(*lit,*lit);	  
	  net->addSyncPart(*lit,instanceName(i),*lit);
	}
	for (labels_it lit = it->getNextLabels().begin() ; 
	     lit != it->getNextLabels().end() ;
	     ++lit) {
	  net->addSynchronization(*lit,*lit);	  
	  net->addSyncPart(*lit,instanceName(next),*lit);
	}
      }
    }

    // build a representation of N = n*grain + complement instances
    // with grain > complement. Normally used with N/grain = n, N % grain = complement.
    // encoding uses $complement$ blocks of $grain+1$ instances
    // and n-complement blocks of n
    void buildNaryRepresentation (size_t n , int grain=1, int complement =0,bool close_loop=false){
      int N = n*grain + complement;
      
      Composite net (typeName(N));
      for (int i=0 ; i < complement ; ++i) {
	net.addInstance(instanceName(i),typeName(grain+1),model_);
      }
      for (size_t i=complement; i< n; ++i) {
	net.addInstance(instanceName(i),typeName(grain),model_);
      }
      
      // delegators
      for (ScalarSet::syncs_it it = comp_.syncs_begin() ; it != comp_.syncs_end() ; ++it ){
	Label sname = it->getName();
	Label slabel = it->getLabel();
	if ( it->isALL() ) {
	  // handle it as a single sync with $n$ parts
	  if (! close_loop && slabel == "") {
	  // if isALL and the EXTERNAL delegator is Private visibility
	  // then we should use public labels in internal components and only use private for
	  // the most external component (as identified by the close_loop parameter)
	     net.addSynchronization( sname, sname );
	  } else {
	    net.addSynchronization( sname , slabel );
	  }
	 
	  for (size_t i=0 ; i < n ; ++i) {
	    net.addSyncPart (sname , instanceName(i), sname);
	  }
	} else {
	  // handle it as $n$ syncs each with a single part
	  for (size_t i=0 ; i < n ; ++i) {
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
	for (size_t i = 0; i < n ; ++i) {
	  net.updateStateDef (stname , instanceName(i), it->second.getState(i) );
	}

      }
      // Circular set specific handling
      if (const CircularSet * ccomp = getCComp()) {
	addInternal(&net,ccomp,n);
	if (close_loop) {
	  addLoop(&net,ccomp,n);
	} else {
	  addEdges(&net,ccomp,n);
	}
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
 
      // close loop set to true here
      buildNaryRepresentation (n/grain_, grain_, n % grain_, true) ;
      return typeName(n);
    } 
  };

  class DepthRecStrategy : public BasicStrategy {
    double factor_;
  public :
    DepthRecStrategy (const ScalarSet & comp, ITSModel & m, int param = 1, double factor = 1.0): BasicStrategy(comp,m,param), factor_(factor) {};

    virtual ~DepthRecStrategy () {}        

    void buildNWiseComposite (int n, int grain, bool close_loop)  {
      int part = n / grain;
      int rem = n % grain ;
      if (model_.findType(typeName(n)) != NULL)
	return ;
      else if (n < grain || grain == 1) {
	// use grain 1 for final level
	buildNaryRepresentation (n);
	return ;
      } else if (part > 1)
	buildNWiseComposite (part, (int) (grain * factor_) ,false);

      if (rem != 0)
	buildNWiseComposite (part + 1, (int) (grain * factor_) ,false);

//       std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " blocks " 
// 		<< " of sizes " << (grain-1) << "*" << part << ".." << part + rem << std :: endl ;
      buildNaryRepresentation(grain,part,rem, close_loop);
    }

    virtual vLabel buildRepresentation ( ) {
     // clone the basic contained type
      model_.cloneType(comp_.getInstance().getType());
      size_t n = comp_.size();
      buildNWiseComposite(n, grain_,true);
      return typeName(n);
    }

  };


  class ShallowRecStrategy : public BasicStrategy {
  public :
    ShallowRecStrategy (const ScalarSet & comp, ITSModel & m, int param = 1): BasicStrategy(comp,m,param) {};

    virtual ~ShallowRecStrategy () {}    

    void buildNWiseComposite (int n, int grain, bool close_loop)  {
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
	buildNWiseComposite (blocksize + 1, grain , false);
      buildNWiseComposite (blocksize, grain , false);


//       std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " in " << nblocks << " blocks " 
// 		<< " of sizes " << (part+1) << "*" << rem << "+" << part << "*" << nblocks - rem << std :: endl ;
      buildNaryRepresentation(nblocks,blocksize,rem, close_loop);
    }

    virtual vLabel buildRepresentation ( ) {
     // clone the basic contained type
      model_.cloneType(comp_.getInstance().getType());
      size_t n = comp_.size();
      buildNWiseComposite(n, grain_, true);
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
      strat_ = new BasicStrategy(getComp(), model_, parameter);
      break ;
    case DEPTHREC :
      strat_ = new DepthRecStrategy(getComp(),model_,parameter);
      break;
    case SHALLOWREC :
      strat_ = new DepthRecStrategy(getComp(),model_,parameter);
      break;
    default :
      strat_ = new BasicStrategy(getComp(), model_, parameter);
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

