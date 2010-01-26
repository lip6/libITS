// Copyright (C) 2003, 2004, 2006 Laboratoire d'Informatique de Paris
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

#ifndef SPOT_TGBA_SLOGPRODUCT_HH
# define SPOT_TGBA_SLOGPRODUCT_HH

#include "tgba/tgba.hh"
#include "sogIts.hh"

namespace slog
{

  /// \brief A state for spot::tgba_product.
  /// \ingroup tgba_on_the_fly_algorithms
  ///
  /// This state is in fact a pair of state: the state from the left
  /// automaton and that of the right.
  class slog_state : public spot::state
  {
  public:
    /// \brief Constructor
    /// \param left The state from the left automaton.
    /// \param right The state from the right automaton.
    /// These states are acquired by spot::state_product, and will
    /// be deleted on destruction.
    slog_state(spot::state* left, its::State right)
      :	left_(left),
	right_(right)
    {
    }

    /// Copy constructor
    slog_state(const slog_state& o);

    virtual ~slog_state();

    state*
    left() const
    {
      return left_;
    }

    its::State
    right() const
    {
      return right_;
    }

    virtual int compare(const state* other) const;
    virtual size_t hash() const;
    virtual slog_state* clone() const;

  private:
    spot::state* left_;		///< State from the left automaton.
    its::State right_;		///< State from the right automaton.
  };


  /// \brief Iterate over the successors of a product computed on the fly.
  class slog_succ_iterator: public spot::tgba_succ_iterator
  {
  public:
    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
    slog_succ_iterator(const spot::tgba * aut, spot::tgba_succ_iterator* left, const sogIts & model, const its::State& right);

    virtual ~slog_succ_iterator();

    // iteration
    void first();
    void next();
    bool done() const;

    // inspection
    slog_state* current_state() const;
    bdd current_condition() const;
    bdd current_acceptance_conditions() const;

  private:
    //@{
    /// Internal routines to advance to the next successor.
    void step_();
    void next_non_false_();
    bdd compute_weaker_selfloop_ap() ;
    //@}

    // to allow pretty printing of arc annotations
    friend class slog_tgba;
  protected:
    const spot::tgba * aut_;
    spot::tgba_succ_iterator* left_;
    const sogIts & model_; ///< The ITS model.
    its::State right_; ///< The source state.   
    its::State dest_; ///< The current successor aggregate (could be empty).   
  };


  /// \brief A divergent state in some specific cases.
  class slog_div_state : public spot::state {
  public:
    slog_div_state(const bdd& c, const bdd &a);
    int compare(const state* other) const;
    size_t hash() const;
    state* clone() const;
    const bdd& get_condition() const;
    const bdd& get_acceptance() const;

    
    // pretty print
    std::ostream & print (std::ostream &) const ;
    
  private:
    bdd cond; ///< the condition.
    bdd acc; ///< the (full) acceptance set
  };

  class slog_div_succ_iterator : public spot::tgba_succ_iterator
  {
  public:
    slog_div_succ_iterator(const spot::bdd_dict* d,
			   const slog_div_state* s);


    void first();
    void next();
    bool done() const;

    spot::state* current_state() const;
    bdd current_condition() const;
    bdd current_acceptance_conditions() const;
    std::string format_transition() const;

  private:
    slog_div_succ_iterator(const slog_div_succ_iterator& s);
    slog_div_succ_iterator& operator=(const slog_div_succ_iterator& s);

    const spot::bdd_dict* dict;
    const slog_div_state* state;
    bool done_;
  };




  /// \brief A lazy product.  (States are computed on the fly.)
  class slog_tgba : public spot::tgba
  {
  public:
    /// \brief Constructor.
    /// \param left The left automata in the product.
    /// \param right The ITS model.
    slog_tgba(const spot::tgba* left, const sogIts & right);

    virtual ~slog_tgba();

    virtual spot::state* get_init_state() const;

    virtual spot::tgba_succ_iterator*
    succ_iter(const spot::state* local_state,
	      const spot::state* global_state = 0,
	      const spot::tgba* global_automaton = 0) const;

    virtual spot::bdd_dict* get_dict() const;

    virtual std::string format_state(const spot::state* state) const;

    virtual std::string
    transition_annotation(const spot::tgba_succ_iterator* t) const;

    virtual spot::state* project_state(const spot::state* s, const spot::tgba* t) const;

    virtual bdd all_acceptance_conditions() const;
    virtual bdd neg_acceptance_conditions() const;

  protected:
    virtual bdd compute_support_conditions(const spot::state* state) const;
    virtual bdd compute_support_variables(const spot::state* state) const;

  private:
    spot::bdd_dict* dict_;
    const spot::tgba* left_;
    const sogIts & model_;

    // Disallow copy.
    slog_tgba(const slog_tgba&);
    slog_tgba& operator=(const slog_tgba&);
  };

}

#endif // SPOT_TGBA_SLOGPRODUCT_HH
