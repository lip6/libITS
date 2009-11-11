#ifndef __TYPE_BASICS__HH__
#define __TYPE_BASICS__HH__

#include "Type.hh"
#include "Ordering.hh"



namespace its {

/** A class offering some basic type support :
 *  Handles default state definition, and support for gneric variable order storage */
  class TypeBasics : public Type {
    	// mutable so that getVarOrder remains const.
	mutable VarOrder * vorder_;
	// default state label
	vLabel default_;
  protected :
    // also sets to default if currently unset.
    VarOrder * getVarOrder () const;

    // To implement : returns the set of DD variables used
    virtual labels_t getVarSet () const = 0;
  public :
    TypeBasics () : vorder_(NULL) {};
    // delete vorder_
    virtual ~TypeBasics () ;
    /** set variable order in representation.
     * Vars should contain a permutation of all DD variables currently defined (i.e. as queriable through getVarSet()).
     * The construction through "addXXX" order is used by default.
     * Returns true if the update was successful or false if "vars" is malformed.
     * NOTE : do not modify order between calls to getLocals or getSuccs or cache mixup could occur !! */
    bool setVarOrder (labels_t vars) const;
    void printVarOrder (std::ostream & os) const { getVarOrder()->print(os); }

    /** one initial state can be designated as initial state */
    Label getDefaultState() const;
    /** the setter returns false if the label provided is not in InitStates */
    bool setDefaultState (Label def);
    
    // succs, getState, locals left pure virtual
  } ;


} // namespace

#endif
