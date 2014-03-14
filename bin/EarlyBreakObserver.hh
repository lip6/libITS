#ifndef __EARLY_BREAK_OBS_HH__
#define __EARLY_BREAK_OBS_HH__

#include "FixObserver.hh"


#define trace if(verbose) std::cout
//#define trace std::cerr


namespace its {

class EarlyBreakObserver : public fobs::FixObserver {
  bool interrupted;
  size_t n;
  size_t max;
  Transition pred;
  bool verbose;
public:
  EarlyBreakObserver (size_t m, Transition p, bool verbose=false)
  : interrupted(false)
  , n(0)
  , max(m)
  , pred(p)
  , verbose(verbose)
  {}
  
  bool was_interrupted () const { return interrupted; }
  
  bool
  should_interrupt (const GSDD & after, const GSDD & before)
  {
    if (interrupted)
      return true;
    if (n > max)
    {
      if (after != before)
      {
        interrupted = true;
        return true;
      }
    }
    else
    {
      ++n;
    }
    return false;
  }
  
  bool
  should_interrupt (const GDDD & after, const GDDD & before)
  {
    if (interrupted)
      return true;
    if (n > max)
    {
      if (after != before)
      {
        interrupted = true;
        return true;
      }
    }
    else
    {
      ++n;
    }
    return false;
  }

  void update (const GSDD & after, const GSDD & before)
  {
    if (pred (after) != State::null)
    {
      trace << "Found a reachable state matching predicate : " << pred  << "\nWill report total states built up to this point. Computation was interrupted after " << n << " fixpoint passes" << std::endl;
    }
    else
    {
      n = 0;
      max += max;
      interrupted = false;
      trace << "SDD proceeding with computation, new max is " << max << std::endl;
    }
  }
  void update (const GDDD & after, const GDDD & before)
  {
    if (pred (State(0,DDD(after))) != State::null)
    {
      trace << "Found a reachable state matching predicate : " << pred  << "\nWill report total states built up to this point. Computation was interrupted after " << n << " fixpoint passes" << std::endl;
    }
    else
    {
      n = 0;
      max += max;
      interrupted = false;
      //      trace << "DDD proceeding with computation, new max is " << max << std::endl;
    }
  }
};


}

#undef trace
#endif
