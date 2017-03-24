#ifndef __TIME_CONSTYRAINT_HH__
#define __TIME_CONSTYRAINT_HH__

#include <iostream>

namespace its {

/** a struct to store a time constraint for syncs or TPN transitions */
class TimeConstraint {
public :
  int eft;
  int lft;
  static const int CLOCK_INFINITY=-1;
  TimeConstraint (int eft_=0, int lft_=CLOCK_INFINITY) : eft(eft_), lft(lft_) {};

  static vLabel infString (int time) {
    if (time == CLOCK_INFINITY) {
      return "inf";
    } else {
      // to_string from Naming.hh
      return to_string(time);
    }
  }
  

  bool isTimed () const { return ! (eft == 0 && lft == CLOCK_INFINITY) ; }

  void print (std::ostream & os) const  { os << "[ " << eft << "," << infString(lft) << " ]"; }
};


} // namespace


#endif
