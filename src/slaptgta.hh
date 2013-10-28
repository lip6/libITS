// Copyright (C) 2013 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

#ifndef SPOT_TGTA_SLAPPRODUCT_HH
# define SPOT_TGTA_SLAPPRODUCT_HH

#include "ta/tgta.hh"
#include "slap.hh"
#include "etf/ETFTestingType.hh"
#include "fsltl.hh"

namespace slap
{

  /// \brief Iterate over the successors of a product computed on the fly.
  class tgta_slap_succ_iterator : public slap_succ_iterator
  {
  public:
    /** aut : the automaton, passed to allow creation of iterators
     * left : the current succ iter on the autoamaton
     * model : the ITS model
     * right : the source aggregate */
    tgta_slap_succ_iterator(const spot::tgba * aut,
        const spot::state * aut_state, spot::tgba_succ_iterator* left,
        const sogIts & model, const its::State& right, sogits::FSTYPE fsType_);

    //virtual ~tgta_slap_succ_iterator();

    // iteration
    //void first();
    //void next();
    //bool done() const;

    // inspection
    //slap_state* current_state() const;
    //	bdd current_condition() const;
    //bdd current_acceptance_conditions() const;

  protected:
    //@{
    /// Internal routines to advance to the next successor.
    virtual void
    step_();
    //void next_non_false_();
    its::Transition
    compute_weaker_selfloop_trans();
    //@}

    // to allow pretty printing of arc annotations
    friend class slap_tgta;
  protected:
    its::EtfTestingType* model_labled_by_changesets_;
  };

  /// \brief A lazy product.  (States are computed on the fly.)
  class slap_tgta : public slap_tgba
  {
  public:

    slap_tgta(const spot::tgba* left, const sogIts & right,
        sogits::FSTYPE fsType);

    virtual spot::state*
    get_init_state() const;

    virtual spot::tgba_succ_iterator*
    succ_iter(const spot::state* local_state, const spot::state* global_state =
        0, const spot::tgba* global_automaton = 0) const;

  };

}

#endif // SPOT_TGTA_SLAPPRODUCT_HH
