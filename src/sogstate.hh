#ifndef SOG_STATE_HH
#define SOG_STATE_HH

#include "tgba/state.hh"
#include "bdd.h"

/// \brief Implementation of a \a spot::state.
class sog_state : public spot::state {
public:
  sog_state(const bdd& m, bool di);
  int compare(const state* other) const;
  size_t hash() const;
  state* clone() const;
  const bdd& get_marking() const;
  bool get_div() const;
  private:
  bdd ma; ///< the frontier markings.
  bool div; ///< the attribut div.
};

class sog_div_state : public spot::state {
  public:
    sog_div_state(const bdd& c);
    int compare(const state* other) const;
    size_t hash() const;
    state* clone() const;
    const bdd& get_condition() const;
  private:
    bdd cond; ///< the condition.
};

#endif
