#ifndef __OBSERVE__HOM__
#define __OBSERVE__HOM__

#include "its/ITSModel.hh"

namespace its {

  typedef std::vector<int> vars_t;
  typedef vars_t::const_iterator vars_it;
  /** This operation replaces values of variables out of the provided set by their values in "pot". 
   *  We expect "observed" to be sorted in decreasing order, and that the DDD we manipulate 
   *  (both the one we apply to and potential) are similarly arranged (0 being the last variable before 1 terminal). 
   */
  GHom observeVars (const vars_t & observed, GDDD potential);

}

#endif
