// Copyright (C) 2003, 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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

#ifndef SPOT_KRIPKE_KRIPKE_HH
# define SPOT_KRIPKE_KRIPKE_HH

#include "state.hh"
#include "kripke_succiter.hh"
#include "bdddict.hh"

namespace spot
{
  /// \defgroup ks KS (Kripke Structure)
  ///
  /// State based verification logic is often founded on the 
  /// formalism Kripke structure.
  ///
  /// \brief A Kripke structure.
  ///
  /// A KS\begin{definition} [Kripke structure]
  ///  Let $\AP$ be a finite set of atomic propositions. 
  /// A \emph{Kripke structure} ($\KS$ for short) over $\AP$ is a $4$-tuple
  ///  $\tuple{\kst,\lab,\tr{},s_0}$ where:
  ///\begin{itemize}
  ///  \item $\kst$ is a finite set of \emph{states} ;
  ///  \item $\lab:\kst \tr{} 2^{\AP}$ is a labeling function;
  ///  \item $\tr{} \subseteq \kst \times \kst$ is a
  ///    \emph{transition relation} ;
  ///  \item $s_0\in\kst$ is the \emph{initial state}.
  ///\end{itemize}
  ///\end{definition}
  ///
  /// 
  /// Browsing such automaton can be achieved using two functions.
  /// \c get_init_state, and \c succ_iter.  The former returns
  /// the initial state while the latter allows to explore the
  /// successor states of any state.
  ///
  class kripke
  {
  protected:
    kripke();

  public:
    virtual ~kripke();

    /// \brief Get the initial state of the automaton.
    ///
    /// The state has been allocated with \c new.  It is the
    /// responsability of the caller to \c delete it when no
    /// longer needed.
    virtual state* get_init_state() const = 0;

    /// \brief Get an iterator over the successors of \a local_state.
    ///
    ///  \item $\tr{} \subseteq \kst \times \kst$ is a
    ///    \emph{transition relation} ;
    ///
    /// The iterator has been allocated with \c new.  It is the
    /// responsability of the caller to \c delete it when no
    /// longer needed.
    ///
    /// \param local_state The state whose successors are to be explored.
    /// This pointer is not adopted in any way by \c succ_iter, and
    /// it is still the caller's responsability to delete it when
    /// appropriate (this can be done during the lifetime of
    /// the iterator).
    virtual kripke_succ_iterator*
    succ_iter(const state* local_state) const = 0;

    /// \brief Get a formula that must hold whatever successor is taken.
    ///
    /// \return A formula which must be verified for all successors
    ///  of \a state.
    ///
    /// This is simply the labeling function : $\lab:\kst \tr{} 2^{\AP}$ is a labeling function;.
    ///
    bdd label (const state* state) const = 0;

    /// \brief Format the state as a string for printing.
    ///
    /// This formating is the responsability of the automata
    /// who owns the state.
    virtual std::string format_state(const state* state) const = 0;

    /// \brief Return a possible annotation for the transition
    /// pointed to by the iterator.
    ///
    /// Implementing this function is optional; the default annotation
    /// it the empty string.
    ///
    /// \param t a non-done kripke_succ_iterator for this automata
    virtual std::string
    transition_annotation(const kripke_succ_iterator* t) const;


  protected:
    /// Do the actual computation of kripke::support_conditions().
    virtual bdd compute_label(const state* state) const = 0;
  };

}

#endif // SPOT_KRIPKE_KRIPKE_HH
