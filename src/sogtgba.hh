// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
// (MoVe), Universite Pierre et Marie Curie.
//
// This file is part of the Spot tutorial. Spot is a model checking 
// library.
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

#ifndef SOG_TGBA_HH
#define SOG_TGBA_HH

#include <string>
#include <map>

#include "tgba/tgba.hh"
#include "tgba/bdddict.hh"
#include "tgba/state.hh"
#include "tgba/succiter.hh"
#include "ltlvisit/apcollect.hh"
#include "tgbaalgos/emptiness_stats.hh"
//#include "ltlast/atomic_prop.hh"
#include "bdd.h"

#include "ITSModel.hh"


namespace sogits {

/// \brief Encapsulation of an ITS model in a
/// \a spot::tgba.
class sog_tgba : public spot::tgba {
public:
  /// \brief Construct a tgba view of the symbolic observation graph of \a pn 
  /// where the set of atomic propositions which are obseved are in \a sap. Notice that 
  /// these atomic propositions are the observed transitions used for the construction. The
  /// constructor registers these propositions in \a dict.
  ///
  /// The pointers \a pn and \a dict are supposed to be valide during all the live of the 
  /// constructed instance.
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline sog_tgba::sog_tgba
  /// \until } //
  sog_tgba(const its::ITSModel & m, 
          spot::bdd_dict* dict);

  /// \brief Unregister all the used propositions.
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline sog_tgba::~sog_tgba
  /// \until } //
  virtual ~sog_tgba();

  /// \brief Get the initial state of the automaton.
  ///
  /// The state has been allocated with \c new.  It is the
  /// responsability of the caller to \c delete it when no
  /// longer needed.
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline spot::state* sog_tgba::get_init_state
  /// \until } //
  spot::state* get_init_state() const;

  /// \brief Get an iterator over the successors of \a local_state.
  ///
  /// The iterator has been allocated with \c new.  It is the
  /// responsability of the caller to \c delete it when no
  /// longer needed.
  ///
  /// The two last parameters are not used here
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline spot::tgba_succ_iterator* sog_tgba::succ_iter
  /// \until } //
  spot::tgba_succ_iterator* succ_iter (const spot::state* local_state,
          const spot::state* gs=0, const spot::tgba* ga=0) const;

  /// \brief Get the dictionary associated to the automaton.
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline bdd_dict* sog_tgba::get_dict
  /// \until } //
  spot::bdd_dict* get_dict() const;

  /// \brief Format the state as a string for printing.
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline std::string sog_tgba::format_state
  /// \until } //
  std::string format_state(const spot::state* state) const;

  /// \brief Format the pointed transition as a string for printing.
  ///
  /// \param t a non-done sog_succ_iterator for this automata
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline std::string sog_tgba::transition_annotation
  /// \until } //
  std::string transition_annotation(const spot::tgba_succ_iterator* t) const;

  /// \brief Return the empty set (false) as the Petri net accepts 
  /// all infinite sequences. Take care that blocking sequences are
  /// not taken into account here.
  bdd all_acceptance_conditions() const;

  /// \brief Return true.
  bdd neg_acceptance_conditions() const;
  
  void clear_stat();
  const spot::unsigned_statistics* get_stat() const;
  void set_stat(bool b);

protected:
  /// Do the actual computation of tgba::support_conditions(). Return true.
  bdd compute_support_conditions(const spot::state* state) const;

  /// Do the actual computation of tgba::support_variables(). Return true.
  bdd compute_support_variables(const spot::state* state) const;

private:
  bdd condition(const bdd& m) const;
  
  /// \brief not implemented (assert(false))
  sog_tgba(const sog_tgba& p);
  /// \brief not implemented (assert(false))
  sog_tgba& operator=(const sog_tgba& p);
  

  /// the encapsulated Petri net. 
  const its::ITSModel & model;
  
  /// Point to the associated dictionnary.
  spot::bdd_dict* dict;
  
  /// Map the indexes of atomic propositions to the 
  /// corresponding indexes of bdd variables. 
  std::map<int, int> mplace_at_prop;
  
  class bdd_statistics: public spot::unsigned_statistics {
  public:
    bdd_statistics() : bdd_nodes_(0) {
      stats["bdd nodes"] =
          static_cast<spot::unsigned_statistics::unsigned_fun>(&bdd_statistics::bdd_nodes);
    }
    void set_bdd_nodes(unsigned n) {
      bdd_nodes_ = n;
    }
    unsigned bdd_nodes() const {
      return bdd_nodes_;
    }
    private:
      unsigned bdd_nodes_;
  };
  mutable bdd_statistics  bdd_stat;
};


} // namespace

#endif

