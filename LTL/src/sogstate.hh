#ifndef SOG_STATE_HH
#define SOG_STATE_HH

#include "sogIts.hh"
#include <spot/twa/twa.hh>
#include <bddx.h>
#include "ddd/SDD.h"


namespace sogits {

/// \brief Implementation of a \a spot::state.
class sog_state : public spot::state {
public:
  // build the agregate inductively defined by
  //  (ap&Trans + id)^*  & ap (m)
  sog_state(const sogIts & model, const GSDD & m, bdd ap);
  int compare(const state* other) const;
  size_t hash() const;
  state* clone() const;

  // return states contained in this agregate : that verify AP
  const GSDD & get_states() const;
  // return true if div or dead is true in this agregate
  bool get_div() const;
  // return any successor of a state in get_states such that AP is not true
  GSDD get_succ() const;
  // return the formula over AP used to create this agregate
  bdd get_condition() const { return condition;}

  // pretty print
  std::ostream & print (std::ostream &) const ;

  private:
  SDD states; ///< the frontier markings.
  SDD succ;
  bdd condition;
  bool div; ///< the attribut div.
};

class sog_div_state : public spot::state {
  public:
    sog_div_state(const bdd& c);
    int compare(const state* other) const;
    size_t hash() const;
    state* clone() const;
    const bdd& get_condition() const;

  // pretty print
  std::ostream & print (std::ostream &) const ;

  private:
    bdd cond; ///< the condition.
};


}

std::ostream & operator << (std::ostream & , const sogits::sog_state &);
std::ostream & operator << (std::ostream & , const sogits::sog_div_state &);


#endif
