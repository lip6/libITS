// Copyright (C) 2012 Laboratoire de Recherche et Developpement de l'Epita
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

#ifndef SPOT_SOGKRIPKE_KRIPKE_HH
#define SPOT_SOGKRIPKE_KRIPKE_HH

#include "kripke/kripke.hh"
#include "sogtgba.hh"

namespace sogits
{


  /// \brief Interface for a Kripke structure
  /// \ingroup kripke
  ///
  /// A Kripke structure is a graph in which each node (=state) is
  /// labeled by a conjunction of atomic proposition.
  ///
  /// Such a structure can be seen as spot::tgba without
  /// any acceptance condition.
  ///
  /// A programmer that develops an instance of Kripke structure needs
  /// just provide an implementation for the following methods:
  ///
  ///   - kripke::get_init_state()
  ///   - kripke::succ_iter()
  ///   - kripke::state_condition()
  ///   - kripke::format_state()
  ///   - and optionally kripke::transition_annotation()
  ///
  /// The other methods of the tgba interface (like those dealing with
  /// acceptance conditions) are supplied by this kripke class and
  /// need not be defined.
  ///
  /// See also spot::kripke_succ_iterator.
  class sog_kripke: public spot::kripke
  {
  public:
    ~sog_kripke();
    sog_kripke(sog_tgba*);


        spot::bdd_dict* get_dict() const;
        spot::state* get_init_state() const;

        /// \brief Allow to get an iterator on the state we passed in
        /// parameter.
        spot::tgba_succ_iterator*
        succ_iter(const spot::state* local_state,
    	      const spot::state* global_state = 0,
    	      const tgba* global_automaton = 0) const;

        /// \brief Get the condition on the state
        bdd state_condition(const spot::state* s) const;


        /// \brief Return the name of the state.
        std::string format_state(const spot::state*) const;
  protected:
        sog_tgba* sog_tgba_;

  };
}

#endif // SPOT_SOGKRIPKE_KRIPKE_HH
