#ifndef SOG_SUCC_ITERATOR_HH
#define SOG_SUCC_ITERATOR_HH

#include <string>
#include <map>

#include <spot/twa/twa.hh>
#include <bddx.h>
#include "apiterator.hh"

#include "sogIts.hh"
#include "sogstate.hh"


namespace sogits {

/// \brief Implementation of a \a spot::tgba_succ_iterator for a \a ::marking.
class sog_succ_iterator : public spot::twa_succ_iterator {
  public:
  sog_succ_iterator(const sogIts & m, const sog_state& s);
  virtual ~sog_succ_iterator();

    bool first() override;
    void step();
    bool next() override;
    bool done() const override;
    spot::state* dst() const override;
    bdd cond() const override;
    int current_transition() const;
    spot::acc_cond::mark_t acc() const override;

  // pretty print
  std::ostream & print (std::ostream &) const ;
private:

  const sogIts& model; ///< The petri net.
  const sog_state & from; ///< The source state.

  APIterator * it;
  bool div_needs_visit;
  its::State succstates;
  sog_state * current_succ;
};

class sog_div_succ_iterator : public spot::twa_succ_iterator {
  public:
    sog_div_succ_iterator(const spot::bdd_dict_ptr& d, const bdd& c);

    bool first() override;
    bool next() override;
    bool done() const override;
    spot::state* dst() const override;
    bdd cond() const override;
    int current_transition() const;
    spot::acc_cond::mark_t acc() const override;
    std::string format_transition() const;

  private:
    sog_div_succ_iterator(const sog_div_succ_iterator& s) = delete;
    sog_div_succ_iterator& operator=(const sog_div_succ_iterator& s) = delete;

    const spot::bdd_dict_ptr& dict;
    bdd cond_; ///< The condition which must label the unique successor.
    bool div_has_been_visited;
};


} // namespace sogits


std::ostream & operator << (std::ostream & , const sogits::sog_succ_iterator &);


#endif
