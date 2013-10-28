// Copyright (C) 2013 Laboratoire de Recherche et
// Développement de l'Epita (LRDE)..
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

#ifndef SPOT_TGTA_DSOGPRODUCT_HH
# define SPOT_TGTA_DSOGPRODUCT_HH

#include "dsog.hh"

namespace dsog
{
  class dsog_tgta;

  class initial_dsog_state : public dsog_state
  {
  public:
    /// \brief Constructor
    /// \param left The state from the left automaton.
    /// \param right The state from the right automaton.
    /// These states are acquired by spot::state_product, and will
    /// be deleted on destruction.
    initial_dsog_state(const spot::state* left, const sogIts & model,
        its::State right, bdd bddAP) :
      dsog_state(left, model, its::State::null, bddfalse)
    {
      left_ = left;
      cond_ = bddAP;
      right_ = right;
      div_ = false;
      succ_ = right;
    }

    initial_dsog_state(const initial_dsog_state& o) :
      dsog_state(o)
    {
    }

  };

  /// \brief Iterate over the successors of a product computed on the fly.
  class dsog_tgta_succ_iterator : public dsog_succ_iterator
  {
  public:
    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
    dsog_tgta_succ_iterator(const dsog_tgta* aut, const dsog_state* s,
        spot::tgba_succ_iterator* left_iter_, const sogIts & model);

    // iteration
    virtual void
    step();

  private:
    // to allow pretty printing of arc annotations
    friend class dsog_tgta;

  };

  /// \brief A lazy product.  (States are computed on the fly.)
  class dsog_tgta : public dsog_tgba
  {
  public:
    /// \brief Constructor.
    /// \param left The left automata in the product.
    /// \param right The ITS model.
    dsog_tgta(const spot::tgba* left, const sogIts & right);

    virtual spot::state*
    get_init_state() const;

    virtual spot::tgba_succ_iterator*
    succ_iter(const spot::state* local_state, const spot::state* global_state =
        0, const spot::tgba* global_automaton = 0) const;
  };

}

#endif // SPOT_TGTA_DSOGPRODUCT_HH
