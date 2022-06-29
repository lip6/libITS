/*
 * Scalarset.hh
 *
 *  Created on: 3 dec. 2008
 *      Author: yann
 */

#ifndef SCALARSET_HH_
#define SCALARSET_HH_

#include "its/ITSModel.hh"
#include "its/Type.hh"
#include "its/Naming.hh"
#include "its/scalar/Delegator.hh"
#include "its/scalar/ScalarState.hh"

namespace its {

class ScalarSet : public NamedElement {
protected :
  // the name prefix of contained instances
  Instance comp_;
  // the number of contained instances
  size_t size_;
public :
  // delegators
  typedef std::vector<Delegator> syncs_t;
  typedef syncs_t::const_iterator syncs_it;
  // states
  // a private type to store a set of named scalarset states
  typedef std::map<vLabel,ScalarState> cstates_t;
  typedef cstates_t::const_iterator cstates_it;

private :
  // delegators
  syncs_t syncs_;
  // the markings defined for this net
  cstates_t cstates_;
public :
  ScalarSet (Label name) : NamedElement(name),comp_("",NULL),size_(1) {};
  virtual ~ScalarSet() {};

  cstates_it cstates_begin () const { return cstates_.begin() ; }
  cstates_it cstates_end () const { return cstates_.end() ; }

  syncs_it syncs_begin () const { return syncs_.begin() ; }
  syncs_it syncs_end () const { return syncs_.end() ; }



  /** Set the size of this scalar set. */
  void setSize (size_t size) { size_ = size; }
  size_t size() const { return size_ ; }
  
  const Instance & getInstance() const { return comp_ ; }
  /** Sets the typical instance contained in this scalarset, 
   * returns false if the instance has previously been set 
   * or if the type name is unknown in the model. 
   */
  bool setInstance (Label name, Label type_name, const ITSModel & model);
  /** Add a delegator synchronization to this scalarset.
   * PRIVATE delegators (i.e. empty string "" label) may be fired in isolation (mostly used with ALL option)
   * PUBLIC delegators (i.e. bearing a label) are offered to external synchronization
   * ALL delegators synchronize all contained instances with && composition semantics, that is all parts must fire congruently
   * ANY syncs use || (or actually XOR) composition semantics, that is one and only one part will fire 
   * Specify name (should exist in the contained instance type), visibility, ALL or ANY */
    bool addDelegator (Label sname, Label slabel, bool isALL=true);

    /** Set an assignment in a state.
     * Effect is, for any contained instance i : state(i) = substate
     * This is equivalent to calling  createStateDef(state, labels_t(n,substate))
     */
  bool createStateDef (Label state, Label substate);

  /** Set an assignment in a state.
   * Effect is, for any contained instance i : state(i) = substates[i]
   */
  bool createStateDef (Label state, labels_t substates);

  /** Sets the state of n instances to the specified substate. */
  //  bool updateStateDef (Label state, int n , Label substate);

  virtual std::ostream & print (std::ostream & os) const ;
};


} // namespace

#endif /* SCALARSET_HH_ */
