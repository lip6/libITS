// Copyright (C) 2013, 2016 Laboratoire de Recherche et
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

#include "dsogtgta.hh"
#include <spot/twa/bddprint.hh>

//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

using namespace spot;

namespace dsog
{

  ////////////////////////////////////////////////////////////
  // dsog_succ_iterator

  /** aut : the automaton, passed to allow creation of iterators
   * left : the current succ iter on the autoamaton
   * model : the ITS model
   * right : the source aggregate */
  dsog_tgta_succ_iterator::dsog_tgta_succ_iterator(const dsog_tgta* aut,
      const dsog_state* s, spot::twa_succ_iterator* left_iter,
      const sogIts & model) :
    dsog_succ_iterator(aut, s, left_iter, model)
  {
  }

  void
  dsog_tgta_succ_iterator::step()
  {
    if (has_div) // divergence
      {
        trace << "finding next diverging transition" << std::endl;

        bdd cond = cur->get_cond();
        while ((!left_iter_->done()) && !bdd_implies(bdd_setxor(cond, cond),
						     left_iter_->cond()))
          left_iter_->next();

        if (!left_iter_->done())
          return;

        trace << "divergence done" << std::endl;

        has_div = false;

        if (cur->get_succ() == its::State::null)
          {
            // left_iter_ is already done().
            return;
          }

        left_iter_->first();
      }

    trace << "find next regular transition" << std::endl;

    // regular transitions
    do
      {
        if (itap != 0)
          {
            if (!itap->done())
              itap->next();
            if (itap->done())
              {
                left_iter_->next();
                delete itap;
                itap = 0;
                if (left_iter_->done())
                  return;
              }
          }
        if (itap == 0)
          {
            succstates_ = its::State::null;
            bdd ap_src = aut_->ap_reachable_from_left(left_);
            while (succstates_ == its::State::null)
              {
                bdd changeset;
                bdd dont_care_changeset = left_iter_->cond();
                bdd succstates_cond = bddfalse;
                while ((changeset = bdd_satoneset(dont_care_changeset, ap_src,
                    bddtrue)) != bddfalse)
                  {
                    dont_care_changeset -= changeset;
                    trace << "step(): compute succstates_ agrega"
                        << "----> changeset = " << bdd_format_formula(
                        aut_->get_dict(), changeset) << std::endl;
                    succstates_cond |= bdd_setxor(cond(), changeset);
                  }

                its::Transition selector = model_.getSelector(succstates_cond);
                succstates_ = selector(cur->get_succ());

                if (succstates_ == its::State::null)
                  {
                    left_iter_->next();
                    if (left_iter_->done())
                      return;
                  }

              }

            bdd ap = aut_->ap_reachable_from_left(left_iter_->dst());

            sogits::APIterator::varset_t vars;

            // Convert ap into a vector of variable numbers.
            while (ap != bddtrue)
              {
                vars.push_back(bdd_var(ap));
                ap = bdd_high(ap);
              }

            delete itap;
            itap = sogits::APIteratorFactory::create_new(vars);

            /// position "it" at first of ap bdd set
            itap->first();

            // Nothing to split.
            if (vars.empty())
              {
                //assert(itap->done());

                delete dest_;
                dest_ = new dsog_state(left_iter_->dst(), model_,
				       succstates_, bddtrue);
                assert(dest_->right() != its::State::null);
                return;
              }
          }

        // iterate until a non empty succ is found (or end reached)
        for (; !itap->done(); itap->next())
          {
            delete dest_;
            dest_ = new dsog_state(left_iter_->dst(), model_,
				   succstates_, itap->current());
            if (dest_->right() != its::State::null)
              return;
          }
      }
    while (!left_iter_->done());
  }

  ////////////////////////////////////////////////////////////
  // dsog_tgta

  /// \brief Constructor.
  /// \param left The left automata in the product.
  /// \param right The ITS model.
  dsog_tgta::dsog_tgta(const spot::const_twa_graph_ptr& left,
		       const sogIts & right) :
    dsog_tgba(left, right)
  {
  }

  state*
  dsog_tgta::get_init_state() const
  {
    const state* lis = left_->get_init_state();
    bdd ap = ap_reachable_from_left(lis);
    trace << " dsog_tgta::get_init_state():" << left_->format_state(lis)
        << "----> ap = " << bdd_format_formula(left_->get_dict(), ap)
        << std::endl;
    its::State m0 = model_.getInitialState();
    dsog_state *init = new initial_dsog_state(lis, model_, m0, bdd_setxor(ap,
        ap));
    return init;

  }

  spot::twa_succ_iterator*
  dsog_tgta::succ_iter(const state* local_state) const
  {
    const dsog_div_state* d = dynamic_cast<const dsog_div_state*> (local_state);
    if (d)
      {
        twa_succ_iterator* li = left_->succ_iter(d->get_left_state());

        bdd cond = d->get_condition();
        return new dsog_div_succ_iterator(this, bdd_setxor(cond, cond), li);
      }

    const dsog_state* s = dynamic_cast<const dsog_state*> (local_state);
    assert(s);

    twa_succ_iterator* li = left_->succ_iter(s->left());

    return new dsog_tgta_succ_iterator(this, s, li, model_);
  }

}
