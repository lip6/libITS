#ifndef __TYPE_BASICS__HH__
#define __TYPE_BASICS__HH__

#include "its/Type.hh"
#include "its/Ordering.hh"



namespace its {

  // hold the result of a parse
  class AtomicPredicate {
  public :
    int var;
    GHom (* comp) (int,int);
    int val;
    AtomicPredicate(int var, GHom (* comp) (int,int), int val) : var(var),comp(comp),val(val) {}
  };

  /** A class offering some basic type support :
   *  Handles default state definition, and support for gneric variable order storage */
  class TypeBasics : public Type {
    	// mutable so that getVarOrder remains const.
	mutable VarOrder * vorder_;
	// default state label
	vLabel default_;
  public :
    // also sets to default if currently unset.
    VarOrder * getVarOrder () const;
  protected:
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
    virtual bool setVarOrder (labels_t vars) const;
    void printVarOrder (std::ostream & os) const { getVarOrder()->print(os); }
    virtual void addFlatVarSet (labels_t & vars, Label prefix) const ;

    /** one initial state can be designated as initial state */
    Label getDefaultState() const;
    /** the setter returns false if the label provided is not in InitStates */
    bool setDefaultState (Label def);
    
    // succs, getState, locals left pure virtual

    /** The state predicate function : string p -> SHom.
     *  returns a selector homomorphism that selects states verifying the predicate 'p'.
     *  The syntax of the predicate is left to the concrete type realization.
     *  The only constraint is that the character '.' is used as a namespace separator
     *  and should not be used in the concrete predicate syntax.
     *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
    virtual Transition getPredicate (char* predicate) const ;

    /** Called by getPredicate above, which handles boolean conjunction and disjunction.*
     *  Stands for getAtomicPredicate. */
    virtual Transition getAPredicate (Label predicate) const = 0;


    /** Return the index of a given variable in the representation, actually a vector of indices in SDD case.
   */
    virtual void getVarIndex(varindex_t & index, Label vname) const {
      index.push_back(getVarOrder()->getIndex(vname));
    }

    // helper function
    AtomicPredicate parseAtomicPredicate (Label predicate) const;


    // Some helper functions to print states corresponding to a VarOrder.
    // This for a system whose variables are integer domain, coded as single level DDD
    static long printSDDState (State s, std::ostream & os, long limit, const VarOrder & vo) ;
    // This for a system whose variables are integer domain, coded as single level SDD bearing the state as a DDD
    static long printDDDState (State s, std::ostream & os, long limit, const VarOrder & vo) ;
  } ;

  


} // namespace

#endif
