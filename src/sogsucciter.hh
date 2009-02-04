#ifndef SOG_SUCC_ITERATOR_HH
#define SOG_SUCC_ITERATOR_HH

#include <string>
#include <map>

#include "tgba/bdddict.hh"
#include "tgba/succiter.hh"
#include "tgba/state.hh"

#include "bdd.h"

#include "ITSModel.hh"
#include "sogstate.hh"


namespace sogits {

/// \brief Implementation of a \a spot::tgba_succ_iterator for a \a ::marking.
class sog_succ_iterator : public spot::tgba_succ_iterator {
  public:
  sog_succ_iterator(const its::ITSModel & m, const sog_state& s);
    void first();
    void next();
    bool done() const;
    spot::state* current_state() const;
    bdd current_condition() const;
    int current_transition() const;
    bdd current_acceptance_conditions() const;
    std::string format_transition() const;
  
private:

  const its::ITSModel& model; ///< The petri net.
  const sog_state & from; ///< The source state.

  std::set<int>::const_iterator it;
  bool div_has_been_visited;
};

class sog_div_succ_iterator : public spot::tgba_succ_iterator {
  public:
    sog_div_succ_iterator(const spot::bdd_dict* d, const bdd& c);

    void first();
    void next();
    bool done() const;
    spot::state* current_state() const;
    bdd current_condition() const;
    int current_transition() const;
    bdd current_acceptance_conditions() const;
    std::string format_transition() const;

  private:
    sog_div_succ_iterator(const sog_div_succ_iterator& s);
    sog_div_succ_iterator& operator=(const sog_div_succ_iterator& s);

    const spot::bdd_dict* dict;
    bdd cond; ///< The condition which must label the unique successor.
    bool div_has_been_visited;
};


} // namespace sogits 

#endif
