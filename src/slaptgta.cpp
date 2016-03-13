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

#include "slaptgta.hh"
#include <string>
#include <cassert>
#include <spot/misc/hashfunc.hh>
#include <spot/twa/bddprint.hh>
#include "tgbaIts.hh"

using namespace spot;

// #define trace std::cerr
#define trace if (0) std::cerr

namespace slap
{

  ////////////////////////////////////////////////
  // tgta_slap_succ_iterator


  tgta_slap_succ_iterator::tgta_slap_succ_iterator(const spot::twa* aut,
      const spot::state * aut_state, spot::twa_succ_iterator* left,
      const sogIts & model, const its::State& right, sogits::FSTYPE fsType_) :
    slap_succ_iterator(aut, aut_state, left, model, right, fsType_)
  {
    model_labled_by_changesets_ = (its::EtfTestingType*) model.getType();
  }

  its::Transition
  tgta_slap_succ_iterator::compute_weaker_selfloop_trans()
  {
    // The acceptance condition labeling the arc of the tgba
    spot::acc_cond::mark_t ac = left_->acc();
    // The state reached in the tgba through this arc
    const state * q2 = left_->dst();

    // Iterate over q2's successors, and add to selfLoopsTrans the its::Transitions which are on arcs "weaker" (wrt ac)
    its::Transition selfLoopsTrans = its::Transition::null;

    for (auto it: aut_->succ(q2))
      {
        const state * dest = it->dst();

        // Test self loop
        if (dest->compare(q2) == 0)
          {
            // Test ac=>ac' (subsume the arc)
	    spot::acc_cond::mark_t acprime = it->acc();
            if ((ac & acprime) == acprime)
              selfLoopsTrans = selfLoopsTrans
                  + model_labled_by_changesets_->getLocalsByChangeSet(it->cond());
          }
        dest->destroy();
      }
    q2->destroy();

    return selfLoopsTrans;
  }

  void
  tgta_slap_succ_iterator::step_()
  {
    const state * tgta_artificial_init_state = aut_->get_init_state();
    if (aut_state_->compare(tgta_artificial_init_state) == 0)
      {

        //case 1: aut_state_ is the artificial initial state of the TGTA
        its::Transition apcond = model_.getSelector(left_->cond());
        dest_ = apcond(right_);
        if (dest_ != its::State::null)
          {
            trace << "case 1:" << aut_->format_state(aut_state_)
                  << "----> left_->cond()" << bdd_format_formula(
                  aut_->get_dict(), left_->cond()) << "----> "
                  << aut_->format_state(left_->dst()) << std::endl;
            trace << "right_==model_.getInitialState()" << (right_
                  == model_.getInitialState()) << std::endl;
            trace << "apcond(model_.getInitialState())" << apcond(
                  model_.getInitialState()) << std::endl;
            trace << "apcond(right_)" << apcond(right_) << std::endl;

            // Iterate over init states successors, and add to F the atomic props which are on arcs without acceptance conds
            const state * init_tgta = left_->dst();
            its::Transition selfLoopsTrans = its::Transition::id;
	    for (auto it: aut_->succ(init_tgta))
              {
                const state * dest = it->dst();
                // Test self loop
                if (dest->compare(init_tgta) == 0)
                  {
                    // Test ac=0 (empty acceptance cond arcs)
                    if (it->acc() == 0U && aut_->num_sets() > 0)
		      selfLoopsTrans = selfLoopsTrans
			+ model_labled_by_changesets_->getLocalsByChangeSet(it->cond());
                  }
                dest->destroy();
              }
            init_tgta->destroy();

            its::Transition sat = fixpoint(selfLoopsTrans, true);
            trace
                  << "tgta_slap_succ_iterator:Saturate (pre) least fixpoint under selfLoop changesets : "
                  << selfLoopsTrans << std::endl;
            dest_ = sat(right_);
          }

      }
    else
      {
        //case 2: aut_state_ is not the artificial initial state of the TGTA

        // Test if this TGBA arc is a self-loop without acceptance conditions, if the TGBA has acceptance conds
        if (aut_->num_sets() > 0)
          {
	    spot::acc_cond::mark_t curacc = left_->acc();
            if (curacc == 0U)
              {
                const state * q2 = left_->dst();
                if (q2->compare(aut_state_) == 0)
                  {
                    dest_ = its::State::null;
                    q2->destroy();
                    return;
                  }
                q2->destroy();
              }
          }

        // progress to "entry" states of succ in ITS model
        bdd changeset = left_->cond();
        //trace << "case 2:" << aut_->format_state(aut_state_)
        //  << "----> left_->current_condition()" << bdd_format_formula(
        //aut_->get_dict(), left_->current_condition()) << "----> "
        //<< aut_->format_state(left_->current_state()) << std::endl;
        its::Transition succByChangeset =
            model_labled_by_changesets_->getLocalsByChangeSet(changeset);
        //trace << "right_ : " << right_ << std::endl;
        dest_ = succByChangeset(right_);
        //trace << "succByChangeset(right_) : " << dest_ << std::endl;
        if (dest_.empty())
          return;

        // grab the current subsumed arc conditions
        bdd F = compute_weaker_selfloop_ap();

        trace << "grab the current subsumed self loops arc with changeset="
              << bdd_format_formula(aut_->get_dict(), F) << std::endl;

        if (F == bddfalse)
          return;

        its::Transition selfLoopsTrans =
            model_labled_by_changesets_->getLocalsByChangeSet(F);

        if (selfLoopsTrans == its::Transition::null)
          {
            trace
                  << "computing selfLoopsTrans because it is not in TestingModel Map, where changeset= "
                  << bdd_format_formula(aut_->get_dict(), F) << std::endl;

            trace
                  << "computing selfLoopsTrans: grab the current subsumed self loop transitions"
                  << std::endl;
            selfLoopsTrans = compute_weaker_selfloop_trans();
          }

        if (selfLoopsTrans != its::Transition::null)
          {
            // costly saturate the aggregate
            its::Transition sat = fixpoint(
                selfLoopsTrans + its::Transition::id, true);
            dest_ = sat(dest_);
          }
      }

  }

  slap_tgta::slap_tgta(const spot::const_twa_ptr& left, const sogIts & right,
      sogits::FSTYPE fsType) :
    slap_tgba(left, right, fsType)
  {

    its::EtfTestingType* model_labled_by_changesets_ =
        (its::EtfTestingType*) right.getType();

    // Grab the TGTA labels
    its::TgbaType * tgta_ = new its::TgbaType(left);
    labels_t tgbalabs = tgta_->getTransLabels();

    std::set<bdd, bdd_less_than> all_changesets;

    for (labels_it it = tgbalabs.begin(); it != tgbalabs.end(); ++it)
      {
        its::TgbaType::tgba_arc_label_t arcLab =
            tgta_->getTransLabelDescription(*it);

        // in TGTA, change_set = (source_state_condition) XOR (destination_state_condition)
        bdd change_set = arcLab.first;
        all_changesets.insert(change_set);
      }

    //Statistic sB(initState_, "*** Before compute_transitions_by_changesets");
    //sB.print_line(trace);
    model_labled_by_changesets_->compute_transitions_by_changesets(
        all_changesets);

    delete tgta_;
  }
  ;

  state*
  slap_tgta::get_init_state() const
  {
    return new slap_state(left_->get_init_state(), model_.getInitialState());
  }

  spot::twa_succ_iterator*
  slap_tgta::succ_iter(const state* local_state) const
  {

    // else it should be a normal slap state
    const slap_state* s = dynamic_cast<const slap_state*> (local_state);
    assert(s);

    twa_succ_iterator* li = left_->succ_iter(s->left());

    // trace << "Building succ_iter from state : " << left_->format_state (s->left()) << std::endl;
    return new tgta_slap_succ_iterator(left_.get(), s->left(), li, model_,
				       s->right(), fsType_);
  }

}
