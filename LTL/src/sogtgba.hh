// Copyright (C) 2004, 2016 Laboratoire d'Informatique de Paris 6
// (LIP6), (MoVe), Universite Pierre et Marie Curie.
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


#include <spot/twa/twa.hh>
#include <spot/tl/apcollect.hh>
#include <spot/twaalgos/emptiness_stats.hh>
#include <bddx.h>

#include "sogIts.hh"


namespace sogits {

/// \brief Encapsulation of an ITS model in a
/// \a spot::tgba.
class sog_tgba final : public spot::twa {
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
  sog_tgba(const sogIts & m,
	   const spot::bdd_dict_ptr& dict,
	   sogits::sog_product_type type);

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
  spot::state* get_init_state() const override;

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
  spot::twa_succ_iterator*
    succ_iter(const spot::state* local_state) const override;

  /// \brief Format the state as a string for printing.
  ///
  /// \dontinclude sogtgba.cpp
  /// \skipline std::string sog_tgba::format_state
  /// \until } //
  std::string format_state(const spot::state* state) const override;

  void clear_stat();
  const spot::unsigned_statistics* get_stat() const;
  void set_stat(bool b);

private:
  bdd condition(const bdd& m) const;

  sog_tgba(const sog_tgba& p) = delete;
  sog_tgba& operator=(const sog_tgba& p) = delete;

  /// the encapsulated Petri net.
  const sogIts & model;

  /// The type of this product, currently PLAIN_SOG or BCZ99
  sog_product_type type;

  /// Map the indexes of atomic propositions to the
  /// corresponding indexes of bdd variables.
  std::map<int, int> mplace_at_prop;

  spot::state* create_state (const sogIts & model, const GSDD & m, bdd ap) const ;

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

  typedef std::shared_ptr<sog_tgba> sog_tgba_ptr;

} // namespace

#endif

