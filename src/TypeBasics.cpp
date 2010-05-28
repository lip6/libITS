#include "TypeBasics.hh"
#include <algorithm>

using std::find;



namespace its {

  // dtor
  TypeBasics::~TypeBasics() {
    if (vorder_ != NULL)
      delete vorder_;
  }

/** one initial state can be designated as initial state */
Label TypeBasics::getDefaultState() const {
	return default_;
}

// also sets to default if currently unset.
VarOrder * TypeBasics::getVarOrder () const {
  if (vorder_ == NULL) {
    // obtain variable set thru virtual call
    labels_t vnames = getVarSet ();
    vorder_ = new VarOrder(vnames);
  }
  return vorder_;
}

   /** set variable order in representation.
    * Vars should contain a permutation of all DD variables currently defined (i.e. as queriable through getVarSet()).    */
  bool TypeBasics::setVarOrder (labels_t vars) const {
    getVarOrder();
    return vorder_->updateOrder(vars);
  }

  /** the setter returns false if the label provided is not in InitStates */
bool TypeBasics::setDefaultState (Label def) {
  labels_t states = getInitStates ();
  labels_it it = find (states.begin(),states.end(),def);
  if (it != states.end()) {
    default_ = def;
    return true;
  } else {
    return false;
  }
}



} // namespace
