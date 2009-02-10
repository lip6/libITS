#include <cassert>
#include <algorithm>
#include <ext/hash_map>

#include "sogstate.hh"

//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


namespace sogits {

  sog_state::sog_state(const its::ITSModel & model, const GSDD& entryStates, bdd bddAP) 
    : spot::state(),condition(bddAP) {
    states =  model.leastFixpoint ( entryStates, bddAP);
    div = ( model.getDivergent ( states, bddAP ) != its::State::null );
    succ = ( (! model.getSelector(bddAP)) & model.getNextRel() ) (states);
    
    trace << "------\nBuilt sogstate from " << entryStates.nbStates() << " initial states ;" << std::endl ;
    trace << "With AP =" << bddAP << std::endl;
    trace << "Obtained :" << *this ;
    trace << "(succ == states) =" << (succ == states) << std::endl;
    trace << "------\n";
  } 
  
  GSDD sog_state::get_succ() const {
    return  succ;
  }
  

  int sog_state::compare(const state* other) const {
    const sog_state* m = dynamic_cast<const sog_state*>(other);
    if (!m)
      return 1;
    if (div != m->div)
      return div?1:-1;
    return (states < m->states) ? -1 : 
      (states == m->states) ?  0 : 1 ;
  } //

  size_t sog_state::hash() const {
    return states.hash();
  } //

  spot::state* sog_state::clone() const {
    return new sog_state(*this);
  } //

  const GSDD & sog_state::get_states() const {
    return states;
  } //

  bool sog_state::get_div() const {
    return div;
  }

  std::ostream & sog_state::print (std::ostream & os) const {
    return (os << "(SogState : size =" << get_states().nbStates() << " div=" << get_div () << " succ.size = " << get_succ().nbStates() << ")" << std::endl);
  }


  sog_div_state::sog_div_state(const bdd& c) : cond(c) {
  }

  int sog_div_state::compare(const state* other) const {
    const sog_div_state* m = dynamic_cast<const sog_div_state*>(other);
    if (!m)
      return -1;
    return cond.id() - m->cond.id();
  }

  size_t sog_div_state::hash() const {
    __gnu_cxx::hash<int> H;
    return H(cond.id());
  }

  spot::state* sog_div_state::clone() const {
    return new sog_div_state(*this);
  }

  const bdd& sog_div_state::get_condition() const {
    return cond;
  }


  std::ostream & sog_div_state::print (std::ostream & os) const {
    return (os << "SogDivState " << std::endl);
  }


} // namespace


std::ostream & operator << (std::ostream & os, const sogits::sog_state &s) {
  return s.print(os);
}

std::ostream & operator << (std::ostream & os, const sogits::sog_div_state &s) {
 return s.print(os);
}
