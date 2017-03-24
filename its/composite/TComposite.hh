#ifndef __TCOMPOSITE__HH__
#define __TCOMPOSITE__HH__

#include "its/composite/Composite.hh"

namespace its {

/** A class to represent a Timed composite, i.e. whose instances are linked by a common time flow.
 *  as represented by the "elapse" synchronization expected of every instance.
 */
class TComposite : public Composite {
 public :
  TComposite (Label name) : Composite(name) {
    addSynchronization("elapse","elapse");
  };
    
  // overloaded version adds an elapse sync
  bool addInstance (Label name, Label type_name, const ITSModel & model) {
    if (Composite::addInstance(name,type_name,model)) {
      if (addSyncPart("elapse",name,"elapse"))
	// could fail if the subnet is untimed
	return true;
    }
    return false;
  }
};

} // namespace

#endif

