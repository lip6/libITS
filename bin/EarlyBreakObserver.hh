#ifndef __EARLY_BREAK_OBS_HH__
#define __EARLY_BREAK_OBS_HH__

#include "../its/Property.hh"
#include "ddd/FixObserver.hh"


#define trace if(verbose) std::cout
//#define trace std::cerr


namespace its {

class EarlyBreakObserver : public fobs::FixObserver {
  bool interrupted;
  size_t n;
  size_t max;
  std::vector<Transition> preds;
  std::vector<const Property *> props;
  bool verbose;
public:
  EarlyBreakObserver (size_t m, const std::vector<Property> & oriprops, its::ITSModel & model, bool verbose=false)
  : interrupted(false)
  , n(0)
  , max(m)
  , verbose(verbose)
{
	  for (const Property & p : oriprops) {
		  switch (p.getType()) {
		  case BOUNDS :
		  case ATOM:
			  continue;
		  case INVARIANT:
		  {
			  preds.emplace_back(model.getPredicate("!(" + p.getPred() + ")"));
			  props.push_back(&p);
			  break;
		  }
		  case NEVER :
		  {
			  preds.emplace_back(model.getPredicate( p.getPred() ));
			  props.push_back(&p);
			  break;
		  }
		  case REACH :
		  {
			  preds.emplace_back(model.getPredicate( p.getPred() ));
			  props.push_back(&p);
			  break;
		  }
		  }
	  }
}
  
  bool was_interrupted () const { return interrupted; }
  
  bool
  should_interrupt (const GSDD & after, const GSDD & before)
  {
    if (interrupted)
      return true;
    if (n > max)
    {
        interrupted = true;
        return true;
    }
    else
    { if (after != before)
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
        interrupted = true;
        return true;
    }
    else
    { if (after != before)
         ++n;
    }
    return false;
  }

  void update (const GSDD & after, const GSDD & before)
  {
    int progress = 0;
	  for (int i=props.size()-1 ; i>= 0 ; i--) {
		  auto & pred = preds[i];
		  bool verify = pred.has_image(after) != State::null;
		  if (verify) {
			  auto prop = props[i];
			  if (prop->getType()==REACH) {
				  std::cout << "Reachability property " << prop->getName() << " is true." << std::endl;
			  } else if (prop->getType()==INVARIANT) {
				  std::cout << "Invariant property " << prop->getName() << " does not hold." << std::endl;
			  } else if (prop->getType()==NEVER) {
				  std::cout << "Never property " << prop->getName() << " does not hold." << std::endl;
			  }
			  props.erase(props.begin()+i);
			  preds.erase(preds.begin()+i);
			  progress++;
		  }
	  }
	  if (props.size() == 0) {
		  trace << "Found states matching all" << props.size()  << " target predicate "  << "\nWill report total states built up to this point. Computation was interrupted after " << n << " fixpoint passes" << std::endl;
	  } else {
      n = 0;
      if (progress == 0)
	max += max;
      interrupted = false;
      trace << "SDD proceeding with computation,"<< props.size()<< " properties remain." <<" new max is " << max << std::endl;
      trace << "SDD size :" << before.nbStates() << " after " << after.nbStates() << std::endl;
    }
  }
  void update (const GDDD & after, const GDDD & before)
  {
	  update(State(0,DDD(after)), State(0,DDD(before)));
  }
};


}

#undef trace
#endif
