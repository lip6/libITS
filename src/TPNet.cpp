#include "TPNet.hh"


namespace its {

const int TPNet::INFINITY = -1;
bool TPNet::addClock (Label tname, int min, int max) {
  // don't add clocks that leave the semantics unchanged
  if (min == 0 && max == INFINITY)
    return false;
  trans_it t = findName(tname,transitions_);
  if (t == transitions_.end())
    return false;
  // const introduced because of findName signature, ok to remove.
  const_cast<PTransition &>(*t).addClock(min,max);
  return true;
}



} // namespace
