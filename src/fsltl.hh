#ifndef __FSLTL__HH__
#define __FSLTL__HH__

#include "sogIts.hh"
#include "ITSModel.hh"
#include <spot/twa/twa.hh>
#include "tgbaIts.hh"



namespace its {


  /** This class handles the construction of a synchronized product of an ITSModel with an ITS representation of a TGBA.
   *  It builds a composite adapted to the problem and contains the SCC search algorithm. */
  class fsltlModel : public ITSModel {
  protected :
    // Because we need more knowledge when interrogating the tgba
    const class TgbaType * tgba_;
    // To handle all the atomic proposition interaction with bdd and spot
    sogIts * sogIts_;

    virtual
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
    bool declareType (spot::const_twa_ptr tgba);

    // Build the composed system based on the TGBA and the main instance of the ITSModel.
    // Precondition : we have built a model, invoked setInstance and setInstanceState, and declared a TGBA type.
    // Side-effect : updates the main instance
    // Returns false and abort if any preconditions are unfulfilled
    virtual
    bool buildComposedSystem ();


    /// calls static operation with member data.
    State findSCC_owcty();

    /// calls static operation with member data.
    State findSCC_el();


    /// to represent a set of transition relations as passed to findSCC algorithm.
    typedef std::vector<its::Transition> trans_t;
    typedef trans_t::const_iterator trans_it;

    /// Verify whether there are accepting paths in the product.
    /// Precondition : buildComposedSystem has been invoked.
    /// Returns the set of accepting states + suffix.
    /// Algorithm used is a variant of "one-way catch them young" ('92)
    /// see [Tacas2001 - Is there a best symbolic cycle detection algorithm - Fisler et al.]
    // Arguments : all: the whole transition relation, nextAccs : one transition relation per acceptance condition, init : initial states.
    static State findSCC_owcty (its::Transition nextAll, const trans_t & nextAccs, its::State init);

    /// Verify whether there are accepting paths in the product.
    /// Precondition : buildComposedSystem has been invoked.
    /// Returns the set of accepting states + suffix.
    /// Algorithm used is a variant of Emerson-Lei '82
    /// see [Tacas2001 - Is there a best symbolic cycle detection algorithm - Fisler et al.]
    // Arguments : all: the whole transition relation, nextAccs : one transition relation per acceptance condition, init : initial states.
    static State findSCC_el (its::Transition nextAll, const trans_t & nextAccs, its::State init);


    /** Delegate to the appropriate variant of fsltl. */
    static State findSCC_fsltl (its::Transition nextAll, const trans_t & nextAccs, its::State init, bool isOWCTY);


  };


}



#endif
