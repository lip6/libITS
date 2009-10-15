#include "fsltl.hh"
#include "tgbaIts.hh"
#include "Composite.hh"

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
    pType tgba = findType("TGBA");
    if (! tgba) {
      std::cerr << "TGBA not set when calling \"buildComposedSystem\"\n" ;
      return false;
    }

    // Ok preconditions fulfilled.
    // Build the composite.
    vLabel compname = tgba->getName() + "x" + getInstance()->getType()->getName();
    Composite * comp = new Composite(compname);
    // Instance names
    vLabel syst = "system";
    vLabel aut = "tgba";
    // an instance of the system
    comp->addInstance (syst, getInstance()->getType()->getName(), *this);
    // an instance of the TGBA
    comp->addInstance (aut, tgba->getName(), *this);

    // Grab the TGBA labels
    labels_t tgbalabs = tgba->getTransLabels();

    for (labels_it it = tgbalabs.begin() ; it != tgbalabs.end() ; ++it ) {
      TgbaType::tgba_arc_label_t arcLab = tgba_->getTransLabelDescription(*it);
      
      Transition apcond = sogIts_.getSelector(arcLab.first);
      
      
    }

    return true;
  }

}

