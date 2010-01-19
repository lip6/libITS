#ifndef __FSLTL__HH__
#define __FSLTL__HH__


#include "ITSModel.hh"
#include "tgba/tgba.hh"
#include "tgbaIts.hh"
#include "sogIts.hh"


namespace its {


  /** This class handles the construction of a synchronized product of an ITSModel with an ITS representation of a TGBA.
   *  It builds a composite adapted to the problem and contains the SCC search algorithm. */
  class fsltlModel : public ITSModel {
    // Because we need more knowledge when interrogating the tgba
    const class TgbaType * tgba_;
    // To handle all the atomic proposition interaction with bdd and spot
    sogIts * sogIts_;

    State getInitState () ;

    Transition allTrans_;
    Transition getNextByAll () ;

    Transition getNextByAcc (Label acc) ;

    typedef std::map<std::string,Transition> accToTrans_t;
    typedef accToTrans_t::iterator accToTrans_it;
    accToTrans_t accToTrans_;

  public :
    // ctor
    fsltlModel() : tgba_(NULL), sogIts_(NULL), allTrans_(Transition::null) {};

    void setSogModel ( sogIts * sogits) { sogIts_ = sogits ; }

    // Play factory role for building ITS types from other formalisms
    // Returns false and aborts if type name already exists.
    // Create a type to hold a spot TGBA
    bool declareType (const class spot::tgba * tgba);

    // Build the composed system based on the TGBA and the main instance of the ITSModel.
    // Precondition : we have built a model, invoked setInstance and setInstanceState, and declared a TGBA type.
    // Side-effect : updates the main instance
    // Returns false and abort if any preconditions are unfulfilled
    bool buildComposedSystem ();

    // Verify whether there are accepting paths in the product.
    // Precondition : buildComposedSystem has been invoked.
    // Returns the set of accepting states + suffix.
    State findSCC ();


  };


}



#endif
