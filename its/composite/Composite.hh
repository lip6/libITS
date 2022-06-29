/*
 * Composite.hh
 *
 *  Created on: 3 déc. 2008
 *      Author: yann
 */

#ifndef COMPOSITE_HH_
#define COMPOSITE_HH_

#include "its/ITSModel.hh"
#include "its/Type.hh"
#include "its/Naming.hh"
#include "its/composite/Synchronization.hh"
#include "its/composite/CState.hh"

namespace its {

class Composite : public NamedElement {
public :
  // subcomponents
  typedef std::vector<Instance> comps_t;
  typedef comps_t::const_iterator comps_it;
  // synchronizations
  typedef std::vector<Synchronization> syncs_t;
  typedef syncs_t::const_iterator syncs_it;
  // a private type to store a set of named composite states
  typedef std::map<vLabel,CState> cstates_t;
  typedef cstates_t::const_iterator cstates_it;
  // used to expose certain variables of nested components, making the structure "transparent"
  // Maps names of variables in subcomponents to the name of the subcomponent that contains it
  // Note that two nested components cannot expose the same variable (this is not labeling of states)
  typedef std::map<vLabel,vLabel> exposedvars_t;
  typedef exposedvars_t::const_iterator exposedvars_it;
private :
  // instances contained 
  comps_t comps_;
  // syncs 
  syncs_t syncs_;
  // states
  cstates_t cstates_;
  // exposed vars
  exposedvars_t exposed_;

public :
  cstates_it cstates_begin() const { return cstates_.begin() ; }
  cstates_it cstates_end() const { return cstates_.end() ; }
  cstates_it cstates_find (Label lab) const { return cstates_.find(lab); }

  syncs_it syncs_begin() const { return syncs_.begin() ; }
  syncs_it syncs_end() const { return syncs_.end() ; }

  size_t comps_size() const { return comps_.size() ; }
  comps_it comps_begin() const { return comps_.begin() ; }
  comps_it comps_end() const { return comps_.end() ; }
  comps_it comps_find (Label iname) const { return findName(iname,comps_); }

  /** Return the name of the subcomponent that exposes this variable or "" is variable is unknown. */
  vLabel exposedIn (Label varname) const {
    exposedvars_it it = exposed_.find(varname);
    if (it == exposed_.end() ) {
      return "";
    } else {
      return it->second;
    }
  }
  
  void exposeVarIn (Label varname, Label compname) {
    exposed_[varname] = compname;
  }


  Composite (Label name) : NamedElement(name) {};
  virtual  ~Composite () {}
	/** Add an instance to this composite, returns false if instance name already exists,
	 * or if the type name is unknown in the model. Overloaded in TComposite.
	 */
  virtual bool addInstance (Label name, Label type_name, const ITSModel & model);
	/** Add a synchronization to this composite.
	 * Specify :
	 * name : the identifier of this transition
	 * label : the label of this synchronization, or the empty string "" if none (private synchronization)
	 */
  bool addSynchronization (Label sname, Label slabel);
	 /** A synchronization part : <instanceName,transitionLabel>. 
	  * instanceName can be "self" which has a special meaning.
	  * Checks that the label exists in the instance are performed
	  */
    bool addSyncPart (Label sname, Label subnetname, Label tname);
    /** Set an assignment in a state.
     * Effect is: state(subnet) = substate
     * Will create the state if it does not exist, with default states for other instances. */
    bool updateStateDef (Label state, Label subnet, Label substate);

    virtual std::ostream & print (std::ostream & os) const ;
};


} // namespace

#endif /* COMPOSITE_HH_ */
