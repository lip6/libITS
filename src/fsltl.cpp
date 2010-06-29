#include "fsltl.hh"
#include "tgbaIts.hh"


// #define trace std::cerr
#define trace while (0) std::cerr

namespace its {


  // Create a type to hold a spot TGBA
  bool fsltlModel::declareType (const class spot::tgba * tgba) {
    TgbaType * newtgba = new TgbaType (tgba);
    // store reference for later use
    tgba_ = newtgba;
    // Compute and store the AP set to update the sogIts component
    // ??? how to is in the sogtgbautils file.
    return addType(newtgba);
  }

  
  // Build the composed system based on the TGBA and the main instance of the ITSModel.
  // Precondition : we have built a model, invoked setInstance and setInstanceState, and declared a TGBA type.
  // Side-effect : updates the main instance
  // Returns false and abort if any preconditions are unfulfilled
  bool fsltlModel::buildComposedSystem () {
    // instance not set
    if (getInstance() == NULL) {
      std::cerr << "Main instance state not set when calling \"buildComposedSystem\"\n" ;
      return false;
    }
    // init state unset
    if (getInitialState() == State::null) {
      std::cerr << "Initial state not set when calling \"buildComposedSystem\"\n" ;
      return false;
    }

    // lookup the TGBA
    if (! tgba_) {
      std::cerr << "TGBA not set when calling \"buildComposedSystem\"\n" ;
      return false;
    }

    // Verbose print of built system
    // trace << *this << std::endl;

    // Ok preconditions fulfilled.

    // Grab the TGBA labels
    labels_t tgbalabs = tgba_->getTransLabels();

    for (labels_it it = tgbalabs.begin() ; it != tgbalabs.end() ; ++it ) {
      trace << "label :" << *it << std::endl;
      TgbaType::tgba_arc_label_t arcLab = tgba_->getTransLabelDescription(*it);
      trace << "desc :" << arcLab.first << ":" << arcLab.second << std::endl;      
      Transition apcond = sogIts_->getSelector(arcLab.first, getInstance()->getType());

      labels_t labtodo;
      labtodo.push_back(*it);
      Transition toadd = localApply(tgba_->getSuccs(labtodo),1) & localApply(getInstance()->getType()->getLocals()  & apcond, 0) ;
      
      allTrans_ = allTrans_ + toadd;

      labels_t accs = tgba_->getAcceptanceSet(arcLab.second);
      for (labels_it acc = accs.begin() ; acc != accs.end() ; ++acc) {
	accToTrans_it accit = accToTrans_.find(*acc);
	if (accit == accToTrans_.end()) {
	  // first occurrence
	  accToTrans_ [*acc] = toadd;
	} else {
	  accit->second = accit->second + toadd;
	}
      }

    }      
      
      
//       std::cout << "Built model with initstate :" << getInitState() << std::endl;
//       std::cout << "Transition rel (all) :" << allTrans_ << std::endl;
      
      
      return true;
    }
    
  State fsltlModel::getInitState () {
//    return State(1, getInstance()->getType()->getState("init"), State(0, findType("TGBA")->getState("init")));
    return State(1, findType("TGBA")->getState("init"), State(0, getInstance()->getType()->getState("init")));
  }

  Transition fsltlModel::getNextByAll () {
    return allTrans_;
  }

  Transition fsltlModel::getNextByAcc (Label acc) {
    return accToTrans_[acc];
  }  


  State fsltlModel::findSCC_owcty () {
    trans_t nextAccs;
    for (accToTrans_it accit = accToTrans_.begin() ; accit != accToTrans_.end() ; ++accit ) {
      nextAccs.push_back(accit->second);
      trace << "For acceptance condition  :" <<  accit->first << std::endl ;
    }
    
    return findSCC_owcty (getNextByAll(), nextAccs, getInitState());
  }

  State fsltlModel::findSCC_owcty (its::Transition nextAll, const trans_t & nextAccs, its::State init) {

    State reach = fixpoint (nextAll  + Transition::id, true) ( init ); 
    
//     trace << "Reachable states : " << reach.nbStates();
//     if (reach.nbStates() < 15)
//       trace << reach << std::endl;
    
    State div;
    if (nextAccs.empty()) {
      // No acceptance condition : any cycle is accepting
      div = fixpoint (nextAll, true) (reach);
    } else {
      // Fixpoint on all acceptance conds 
      div = reach;
      State sat = div;
      int j=0;
      do {
	trace << "start of SCC loop, iteration " << j++ << " with nbstates= " << div.nbStates()<<std::endl;
	sat = div;


	// only states that allow a loop and suffixes
	// div = fixpoint (nextAll, true) (div);

	// trace << "After loop detection " << j << " with nbstates= " << div.nbStates()<<std::endl;

	// For each acceptance condition
	for (trans_it accit = nextAccs.begin(); accit != nextAccs.end() ; ++accit) {


	  // Let Next = all transitions (any label or acceptance)
	  // and black = successors that validate the acceptance condition "black"
	  // ( (Next + Id)^* & black ) (states)
	  // NB: creating t before application is necessary to enforce proper ref count
	  // otherwise the composition could be gc'ed by the fixpoint's true attribute.
	  its::Transition  t =  (fixpoint (nextAll + Transition::id,true) & (*accit));

	  div = t (div);

	  trace << "Reduced to " << div.nbStates() << " states." << std::endl;
	}

      } while (div != sat);
    }
    trace << "Divergence ? " << div.nbStates() << std::endl;
    if (div.nbStates() < 15)
      trace << div << std::endl;

    return div;

  }


  }
