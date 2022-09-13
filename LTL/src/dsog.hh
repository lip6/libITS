// Copyright (C) 2009, 2010, 2016 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef SPOT_TGBA_DSOGPRODUCT_HH
# define SPOT_TGBA_DSOGPRODUCT_HH

#include <spot/twa/twa.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/sccinfo.hh>
#include "sogIts.hh"
#include "apiterator.hh"

namespace dsog
{
  class dsog_tgba;
  typedef std::shared_ptr<dsog_tgba> dsog_tgba_ptr;
  typedef std::shared_ptr<const dsog_tgba> const_dsog_tgba_ptr;

  class dsog_div_state final : public spot::state {
  public:
    dsog_div_state(const state* s, const bdd& c);
    ~dsog_div_state();
    int compare(const state* other) const override;
    size_t hash() const override;
    state* clone() const override;
    const bdd& get_condition() const;
    const state* get_left_state() const { return left_state_; }

    // pretty print
    std::ostream & print (std::ostream &) const ;

  private:
    dsog_div_state(const dsog_div_state& c);
    dsog_div_state& operator=(const dsog_div_state& c);

    const state* left_state_;
    bdd cond; ///< the condition.
  };

  /// \brief A state for spot::tgba_product.
  /// \ingroup tgba_on_the_fly_algorithms
  ///
  /// This state is in fact a pair of state: the state from the left
  /// automaton and that of the right.
  class dsog_state : public spot::state
  {
  public:
    /// \brief Constructor
    /// \param left The state from the left automaton.
    /// \param right The state from the right automaton.
    /// These states are acquired by spot::state_product, and will
    /// be deleted on destruction.
    dsog_state(const spot::state* left,
	       const sogIts & model, its::State right, bdd bddAP);

    /// Copy constructor
    dsog_state(const dsog_state& o);

    virtual ~dsog_state();

    bool
    get_div() const
    {
      return div_;
    }

    its::State
    get_succ() const
    {
      return succ_;
    }

    const state*
    left() const
    {
      return left_;
    }

    its::State
    right() const
    {
      return right_;
    }

    bdd
    get_cond() const
    {
      return cond_;
    }

    virtual int compare(const state* other) const override final;
    virtual size_t hash() const override final;
    virtual dsog_state* clone() const override final;

  protected:
    const spot::state* left_;		///< State from the left automaton.
    its::State right_;		///< State from the right automaton.
    bool div_;
    its::State succ_;
    bdd cond_;
  };


  /// \brief Iterate over the successors of a product computed on the fly.
  class dsog_succ_iterator: public spot::twa_succ_iterator
  {
  public:
    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
    dsog_succ_iterator(const dsog_tgba* aut,
		       const dsog_state* s,
		       spot::twa_succ_iterator* left_iter_,
		       const sogIts & model);

    virtual ~dsog_succ_iterator();

    // iteration
    virtual void step();
    bool first() override final;
    bool next() override final;
    bool done() const override final;

    // inspection
    spot::state* dst() const override final;
    bdd cond() const override final;
    spot::acc_cond::mark_t acc() const override final;

  private:

    // to allow pretty printing of arc annotations
    friend class dsog_tgba;
  protected:
    bool has_div;
    const dsog_state* cur;
    const dsog_tgba* aut_;
    const spot::state* left_;
    spot::twa_succ_iterator* left_iter_;
    const sogIts & model_; ///< The ITS model.
    its::State right_; ///< The source state.
    dsog_state* dest_; ///< The current successor aggregate (could be empty).
    its::State succstates_;
    sogits::APIterator* itap;
  };


  class dsog_div_succ_iterator final: public spot::twa_succ_iterator
  {
  public:
    dsog_div_succ_iterator(const dsog_tgba* aut,
			   const bdd& c,
			   twa_succ_iterator* li);

    void step();
    bool first() override;
    bool next() override;
    bool done() const override;
    spot::state* dst() const override;
    bdd cond() const override;
    spot::acc_cond::mark_t acc() const override;
    std::string format_transition() const;

  private:
    dsog_div_succ_iterator(const dsog_div_succ_iterator& s);
    dsog_div_succ_iterator& operator=(const dsog_div_succ_iterator& s);

    const dsog_tgba* aut_;
    bdd cond_; ///< The condition which must label the unique successor.
    spot::twa_succ_iterator* left_iter_;
  };


  /// \brief A lazy product.  (States are computed on the fly.)
  class dsog_tgba: public spot::twa
  {
  public:
    /// \brief Constructor.
    /// \param left The left automata in the product.
    /// \param right The ITS model.
    dsog_tgba(const spot::const_twa_graph_ptr& left, const sogIts & right);

    virtual ~dsog_tgba();

    virtual spot::state* get_init_state() const override;

    /// the conjunction of all APs reachable from the state \a s in
    /// the left automaton.
    bdd ap_reachable_from_left(const spot::state* s) const;

    virtual spot::twa_succ_iterator*
    succ_iter(const spot::state* local_state) const override;

    virtual std::string format_state(const spot::state* state) const
      override final;

    virtual spot::state*
      project_state(const spot::state* s,
		    const spot::const_twa_ptr& t) const override final;

  protected:
    spot::const_twa_graph_ptr left_;
    const sogIts & model_;
    spot::scc_info scc_;
    std::vector<bdd> scc_ap_;

    // Disallow copy.
    dsog_tgba(const dsog_tgba&) = delete;
    dsog_tgba& operator=(const dsog_tgba&) = delete;
  };

}

#endif // SPOT_TGBA_DSOGPRODUCT_HH
