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

#include "dsogtgta.hh"
#include "tgba/bddprint.hh"

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
      const dsog_state* s, spot::tgba_succ_iterator* left_iter,
      const sogIts & model) :
    dsog_succ_iterator(aut, s, left_iter, model)
  {
  }

  /*
   void
   dsog_tgta_succ_iterator::step()
   {
   if (has_div) // divergence
   {
   trace
   << " step():" << "----> has_div = " << has_div << std::endl;
   trace
   << "finding next diverging transition" << std::endl;

   bdd cond = cur->get_cond();
   while ((!left_iter_->done()) && !bdd_implies(bdd_xor(cond, cond),
   left_iter_->current_condition()))
   left_iter_->next();

   //        bdd cond = left_iter_->current_condition();
   //              while ((!left_iter_->done()) && (bdd_xor(cond, cond) != cond))
   //                left_iter_->next();

   if (!left_iter_->done())
   return;

   trace
   << "divergence done" << std::endl;

   has_div = false;

   if (succstates_ == its::State::null)
   {
   // left_iter_ is already done().
   return;
   }

   left_iter_->first();
   }

   trace
   << "find next regular transition" << std::endl;

   // regular transitions
   do
   {
   trace
   << "(do while) after find next regular transition" << std::endl;

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

   const spot::scc_map& scc = aut_->get_scc_map();
   if (itap == 0)
   {
   unsigned sccn = scc.scc_of_state(left_iter_->current_state());
   bdd ap = scc.aprec_set_of(sccn);

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
   trace
   << " Nothing to split" << "----> vars.empty() = "
   << vars.empty() << std::endl;

   //assert(itap->done());
   delete dest_;
   dest_ = new dsog_state(left_iter_->current_state(), model_,
   succstates_, bddtrue);
   assert(dest_->right() != its::State::null);
   return;
   }

   trace
   << " (itap == 0) step():" << "----> itap->current() = "
   << bdd_format_formula(aut_->get_dict(), itap->current())
   << std::endl;

   }

   // iterate until a non empty succ is found (or end reached)
   unsigned sccn_src = scc.scc_of_state(left_);
   bdd ap_src = scc.aprec_set_of(sccn_src);

   for (; !itap->done(); itap->next())
   {
   trace
   << " step():" << "----> dest_ = " << dest_ << std::endl;
   delete dest_;
   dest_ = 0;
   trace
   << " PRE step():" << "----> itap->current() = "
   << bdd_format_formula(aut_->get_dict(), itap->current())
   << std::endl;
   bdd changeset;
   bdd dont_care_changeset = left_iter_->current_condition();
   while ((changeset = bdd_satoneset(dont_care_changeset, ap_src,
   bddtrue)) != bddfalse)
   {
   dont_care_changeset -= changeset;
   trace
   << " PRE step():" << "----> changeset = "
   << bdd_format_formula(aut_->get_dict(), changeset)
   << std::endl;
   if (bdd_implies(bdd_xor(cur->get_cond(), changeset),
   itap->current()))
   {
   trace
   << " POST step():" << "----> changeset = "
   << bdd_format_formula(aut_->get_dict(), changeset)
   << std::endl;
   dest_ = new dsog_state(left_iter_->current_state(), model_,
   succstates_, itap->current());
   if (dest_->right() != its::State::null)
   return;
   }

   }

   }
   }
   while (!left_iter_->done());
   }
   */
  void
  dsog_tgta_succ_iterator::step()
  {
    if (has_div) // divergence
      {
        trace << "finding next diverging transition" << std::endl;

        bdd cond = cur->get_cond();
        while ((!left_iter_->done()) && !bdd_implies(bdd_setxor(cond, cond),
            left_iter_->current_condition()))
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
            const spot::scc_map& scc = aut_->get_scc_map();
            bdd cond = cur->get_cond();
            succstates_ = its::State::null;
            unsigned sccn_src = scc.scc_of_state(left_);
            bdd ap_src = scc.aprec_set_of(sccn_src);
            while (succstates_ == its::State::null)
              {
                bdd changeset;
                bdd dont_care_changeset = left_iter_->current_condition();
                bdd succstates_cond = bddfalse;
                while ((changeset = bdd_satoneset(dont_care_changeset, ap_src,
                    bddtrue)) != bddfalse)
                  {
                    dont_care_changeset -= changeset;
                    trace << "step(): compute succstates_ agrega"
                        << "----> changeset = " << bdd_format_formula(
                        aut_->get_dict(), changeset) << std::endl;
                    succstates_cond |= bdd_setxor(cond, changeset);
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

            unsigned sccn = scc.scc_of_state(left_iter_->current_state());
            bdd ap = scc.aprec_set_of(sccn);

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
                dest_ = new dsog_state(left_iter_->current_state(), model_,
                    succstates_, bddtrue);
                assert(dest_->right() != its::State::null);
                return;
              }
          }

        // iterate until a non empty succ is found (or end reached)
        for (; !itap->done(); itap->next())
          {
            delete dest_;
            dest_ = new dsog_state(left_iter_->current_state(), model_,
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
  dsog_tgta::dsog_tgta(const spot::tgba* left, const sogIts & right) :
    dsog_tgba(left, right)
  {
  }

  state*
  dsog_tgta::get_init_state() const
  {
    const state* lis = left_->get_init_state();
    unsigned sccn = scc_.scc_of_state(lis);
    bdd ap = scc_.aprec_set_of(sccn);
    trace << " dsog_tgta::get_init_state():" << left_->format_state(lis)
        << "----> ap = " << bdd_format_formula(left_->get_dict(), ap)
        << std::endl;
    its::State m0 = model_.getInitialState();
    dsog_state *init = new initial_dsog_state(lis, model_, m0, bdd_setxor(ap,
        ap));
    return init;

  }

  spot::tgba_succ_iterator*
  dsog_tgta::succ_iter(const state* local_state, const state* global_state,
      const tgba* global_automaton) const
  {
    const dsog_div_state* d = dynamic_cast<const dsog_div_state*> (local_state);
    if (d)
      {
        tgba_succ_iterator* li = left_->succ_iter(d->get_left_state(),
            global_state, global_automaton);

        bdd cond = d->get_condition();
        return new dsog_div_succ_iterator(this, bdd_setxor(cond, cond), li);
      }

    const dsog_state* s = dynamic_cast<const dsog_state*> (local_state);
    assert(s);

    tgba_succ_iterator* li = left_->succ_iter(s->left(), global_state,
        global_automaton);

    return new dsog_tgta_succ_iterator(this, s, li, model_);
  }

}
