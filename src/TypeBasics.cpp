#include "TypeBasics.hh"
#include <algorithm>
#include <cstring>

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

  Transition TypeBasics::getPredicate (Label predicate) const {
    char  pred [predicate.size()+1];
    strcpy(pred,predicate.c_str());

    // Test for nesting
    if (* pred == '(') {
      // scan ahead for closing paren.
      char * end = strstr(pred,")");
      if (end == NULL) {
	std::cerr << "Syntax Error : Mismatched paren problem in predicate : " << predicate << std::endl;
	exit(1);
      } else {
	*end='\0';
	Transition nested = getPredicate(pred+1);
	if (end+1!='\0') {
	  if (strncmp(end+1,"&&",2)) {
	    return nested & getPredicate(end+3);
	  } else if (strncmp(end+1,"||",2)) {
	    return nested + getPredicate(end+3);
	  } else {
	    std::cerr << "Syntax Error : Expected operator && or || after predicate : (" << pred+1 << ") but encountered unexpected:" << end+1 << std::endl;
	    exit(1);
	  }
	} else {
	  return nested;
	}
      } 
    } else {
      // No nesting case
      char * and_t = strstr(pred,"&&");
      char * or_t = strstr(pred,"||");
      if (! and_t && ! or_t) {
	return getAPredicate(predicate);
      } 
      if ( and_t && (! or_t || and_t < or_t)) {
	*and_t = '\0';
	Transition left = getPredicate(pred);
	return left & getPredicate(and_t+2);
      } else {
	// so we are sure that :
	// ( or_t && (! and_t || or_t < and_t)) 
	*or_t = '\0';
	Transition left = getPredicate(pred);
	return left + getPredicate(or_t+2);
      }	
    }
    
  }


} // namespace
