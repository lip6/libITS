#include <cassert>
#include <algorithm>
#include <ext/hash_map>

#include "sogstate.hh"


namespace sogits {

  sog_state::sog_state(const its::ITSModel & model, const GSDD& entryStates, bdd bddAP) 
    : spot::state() {
    // states =  model.saturate(entryStates, bddAP);
    // div = model.detectDiv(states, bddAP);
  } 
  
  GSDD sog_state::get_succ() const {
    // compute one step of succs compose with ! selector
    return GSDD::null;
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



} // namespace
