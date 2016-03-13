// Copyright (C) 2003, 2004, 2006, 2016 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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

#ifndef SPOT_TGBA_SLAPPRODUCT_HH
# define SPOT_TGBA_SLAPPRODUCT_HH

#include <spot/twa/twa.hh>
#include "sogIts.hh"
#include "sogtgbautils.hh"



namespace slap
{

  /// \brief A state for spot::tgba_product.
  /// \ingroup tgba_on_the_fly_algorithms
  ///
  /// This state is in fact a pair of state: the state from the left
  /// automaton and that of the right.
  class slap_state final: public spot::state
  {
  public:
    /// \brief Constructor
    /// \param left The state from the left automaton.
    /// \param right The state from the right automaton.
    /// These states are acquired by spot::state_product, and will
    /// be deleted on destruction.
    slap_state(const spot::state* left, its::State right)
      :	left_(left),
	right_(right)
    {
    }

    /// Copy constructor
    slap_state(const slap_state& o);

    virtual ~slap_state();

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

    virtual int compare(const state* other) const override;
    virtual size_t hash() const override;
    virtual slap_state* clone() const override;

  private:
    const spot::state* left_;	///< State from the left automaton.
    its::State right_;		///< State from the right automaton.
  };


  /// \brief Iterate over the successors of a product computed on the fly.
  class slap_succ_iterator: public spot::twa_succ_iterator
  {
  public:
    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
    slap_succ_iterator(const spot::twa* aut, const spot::state* aut_state,
		       spot::twa_succ_iterator* left, const sogIts& model,
		       const its::State& right, sogits::FSTYPE fsType_);

    virtual ~slap_succ_iterator();

    // iteration
    bool first() override final;
    bool next() override final;
    bool done() const override final;

    // inspection
    const slap_state* dst() const override final;
    bdd cond() const override final;
    spot::acc_cond::mark_t acc() const override final;

  protected:
    //@{
    /// Internal routines to advance to the next successor.
    virtual void step_();
    void next_non_false_();
    bdd compute_weaker_selfloop_ap() ;
    //@}

    // to allow pretty printing of arc annotations
    friend class slap_tgba;

    const spot::twa * aut_;
    const spot::state * aut_state_;
    spot::twa_succ_iterator* left_;
    const sogIts & model_; ///< The ITS model.
    its::State right_; ///< The source state.
    its::State dest_; ///< The current successor aggregate (could be empty).
    sogits::FSTYPE fsType_;
  };


  /// \brief A divergent state in some specific cases.
  class slap_div_state final: public spot::state {
  public:
    slap_div_state(const bdd& c, spot::acc_cond::mark_t a);
    int compare(const state* other) const override;
    size_t hash() const override;
    state* clone() const override;
    const bdd& get_condition() const;
    spot::acc_cond::mark_t get_acceptance() const;


    // pretty print
    std::ostream & print (std::ostream &) const ;

  private:
    bdd cond; ///< the condition.
    spot::acc_cond::mark_t acc; ///< the (full) acceptance mark
  };

  class slap_div_succ_iterator final: public spot::twa_succ_iterator
  {
  public:
    slap_div_succ_iterator(const spot::bdd_dict_ptr& d,
			   const spot::state* s, const bdd & cond,
			   spot::acc_cond::mark_t acc);


    bool first() override;
    bool next() override;
    bool done() const override;

    const spot::state* dst() const override;
    bdd cond() const override;
    spot::acc_cond::mark_t acc() const override;
    std::string format_transition() const;

  private:
    slap_div_succ_iterator(const slap_div_succ_iterator& s);
    slap_div_succ_iterator& operator=(const slap_div_succ_iterator& s);

    spot::bdd_dict_ptr dict;
    const spot::state* state;
    bool done_;
    bdd cond_;
    spot::acc_cond::mark_t acc_;
  };




  /// \brief A lazy product.  (States are computed on the fly.)
  class slap_tgba : public spot::twa
  {
  public:
    /// \brief Constructor.
    /// \param left The left automata in the product.
    /// \param right The ITS model.
    slap_tgba(spot::const_twa_ptr left,
	      const sogIts & right,sogits::FSTYPE fsType);

    virtual ~slap_tgba();

    virtual spot::state* get_init_state() const override;

    virtual spot::twa_succ_iterator*
    succ_iter(const spot::state* local_state) const override;

    virtual std::string format_state(const spot::state* state) const override;

    virtual spot::state* project_state(const spot::state* s,
				       const spot::const_twa_ptr& t)
      const override;

  protected:
    spot::const_twa_ptr left_;
    const sogIts & model_;
    sogits::FSTYPE fsType_;

    // test for fully symbolic exploration
    bool isFullAcceptingState (spot::twa_succ_iterator* it, const spot::state * source) const ;
    // true if all it arcs are self loops
    bool isTerminalState (spot::twa_succ_iterator* it, const spot::state * source) const ;
    // true if it->destination == source
    bool isSelfLoop(const spot::twa_succ_iterator* it, const spot::state * source) const ;
    // Disallow copy.
    slap_tgba(const slap_tgba&);
    slap_tgba& operator=(const slap_tgba&);
  };

}

#endif // SPOT_TGBA_SLAPPRODUCT_HH
