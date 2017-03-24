#ifndef __PTNET__HH__
#define __PTNET__HH__

#include "its/petri/PNet.hh"

namespace its {

/** A class to represent a Timed Petri net, "à la" Romeo
 *  Extends basic Petri net PNet concepts with the possibility of
 *  associating a clock to transitions. */
class TPNet : public PNet {
 public :
  TPNet (Label name) : PNet(name) { addTransition("elapse", "elapse", PUBLIC); };
  
  // Clock bounds can be associated to transitions => when this feature is used we actually build a Time Petri net
  // min and max define the min and max bounds of the clock.
  // Max may be set to infinity, changingthe way it is treated slightly.
  // If it is unused, we fall back to usual PNet case.
  // Adding a clock with bounds [0,INF] has no effect.
  // Returns false if tname is not known.
  // \todo static const could be initialization in the header
  static const int CLOCK_INFINITY;
  bool addClock (Label tname, int min, int max);

  static vLabel clockName(Label tname) { return "__clock_"+tname; }
  static vLabel clockName(const PTransition& t) { return clockName(t.getName()); }
};


} // namespace


#endif
