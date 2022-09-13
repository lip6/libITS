#ifndef BCZ_STATE_HH
#define BCZ_STATE_HH


#include <spot/twa/twa.hh>
#include <bddx.h>
#include "ddd/SDD.h"
#include "sogIts.hh"
#include "apiterator.hh"

namespace sogits {

/** This hosts an implementation of Biere, Clarke, Zhu CSD'99 paper.
    Multiple State and Single State Tableaux for Combining Local and Global Model Checking. In E.-R. Olderog, B. Steffen (Eds.), Correct System Design Recent Insights and Advances, special issue dedicated to Hans Langmaack, Lecture Notes in Computer Science (LNCS), vol. 1710, Springer 1999, ext. version of our SMC'99 paper.
*/
/// \brief Implementation of a \a spot::state.
class bcz_state final : public spot::state {
public:
  // build the agregate inductively defined by
  //  (ap&Trans + id)^*  & ap (m)
  bcz_state(const sogIts & model, const GSDD & m, bdd ap);
  int compare(const state* other) const override;
  size_t hash() const override;
  state* clone() const override;

  // return states contained in this agregate : that verify AP
  const GSDD & get_states() const;
  // return any successor of a state in get_states
  GSDD get_succ() const;
  // return the formula over AP used to create this agregate
  bdd get_condition() const { return condition;}

  // pretty print
  std::ostream & print (std::ostream &) const ;

private:
  SDD states; ///< the frontier markings.
  SDD succ;
  bdd condition;
};

/// \brief Implementation of a \a spot::tgba_succ_iterator for a \a ::marking.
class bcz_succ_iterator final : public spot::twa_succ_iterator {
  public:
  bcz_succ_iterator(const sogIts & m, const bcz_state& s);
  virtual ~bcz_succ_iterator();

    bool first() override;
    void step();
    bool next() override;
    bool done() const override;
    spot::state* dst() const override;
    bdd cond() const override;
    spot::acc_cond::mark_t acc() const override;

  // pretty print
  std::ostream & print (std::ostream &) const ;
private:

  const sogIts& model; ///< The petri net.
  const bcz_state & from; ///< The source state.

  APIterator * it;
  its::State succstates;
  bcz_state * current_succ;
};



}

std::ostream & operator << (std::ostream & , const sogits::bcz_state &);


std::ostream & operator << (std::ostream & , const sogits::bcz_succ_iterator &);


#endif
