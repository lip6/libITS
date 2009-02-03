// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
// d�artement Syst�es R�artis Coop�atifs (SRC), Universit�Pierre
// et Marie Curie.
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

#include <cassert>
#include <sstream>

#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/defaultenv.hh"
#include "tgba/bddprint.hh"

#include "sogtgba.hh"
#include "sogsucciter.hh"
#include "sogstate.hh"

sog_tgba::sog_tgba(const petri_net* p, int b, const std::set<int>& obtr, 
        const spot::ltl::atomic_prop_set* sap, spot::bdd_dict* dic) {
  assert(sap && dic);

  pnbdd = new RdPBDD(*p, obtr, b>=1?b:1);

  dict = dic;
  spot::ltl::atomic_prop_set::iterator it;
  for(it = sap->begin(); it != sap->end(); ++it) {
    int num = pnbdd->get_pn().get_place_num((*it)->name());
    mplace_at_prop[num] = dict->register_proposition(*it, this);
  }
} //

void sog_tgba::clear_stat() {
  pnbdd->reset_visited_bdd();
}

const spot::unsigned_statistics* sog_tgba::get_stat() const {
  int res = 0;
  bdd* tab;
  int nb = pnbdd->get_visited_bdd(&tab);
  if (nb) {
    res = bdd_anodecount(tab, nb);
    delete [] tab;
  }
  bdd_stat.set_bdd_nodes(res);
  return &bdd_stat;
}

void sog_tgba::set_stat(bool b) {
  pnbdd->set_collect(b);
}

sog_tgba::sog_tgba(const sog_tgba& p) {
  assert(false);
} //

sog_tgba& sog_tgba::operator=(const sog_tgba& p) {
  assert(false);
  return *this;
} //

sog_tgba::~sog_tgba() {
  if (dict)
    dict->unregister_all_my_variables(this);
  delete pnbdd;
} //

spot::state* sog_tgba::get_init_state() const {
  bool dead, div;
  bdd m0 = pnbdd->get_initial_marking(dead, div);
  assert(m0 != bddfalse);
  return new sog_state(m0, div | dead);
} //

bdd sog_tgba::condition(const bdd& m) const {
  bdd res = bddtrue;
  std::map<int, int>::const_iterator it;
  for (it = mplace_at_prop.begin(); it != mplace_at_prop.end(); ++it)
    if (pnbdd->is_marked(it->first, m))
      res &= bdd_ithvar(it->second);
    else
      res &= bdd_nithvar(it->second);
  return res;
}

spot::tgba_succ_iterator* sog_tgba::succ_iter (const spot::state* local_state,
                                               const spot::state*, const spot::tgba*) const {
  const sog_state* s = dynamic_cast<const sog_state*>(local_state);
  if (s) {
    return new sog_succ_iterator(*pnbdd, *s, condition(s->get_marking()));
  }
  else {
    const sog_div_state* s = dynamic_cast<const sog_div_state*>(local_state);
    assert(s);
    return new sog_div_succ_iterator(dict, s->get_condition());
  }
} //

spot::bdd_dict* sog_tgba::get_dict() const {
  return dict;
} //

std::string sog_tgba::format_state(const spot::state* state) const {
  const sog_state* s = dynamic_cast<const sog_state*>(state);
  if (s) {
    std::string res = pnbdd->format_marking(s->get_marking());
    if (s->get_div())
      res += " (div att.)";
    return res;
  }
  else {
    const sog_div_state* s = dynamic_cast<const sog_div_state*>(state);
    assert(s);
    std::ostringstream os;
    spot::bdd_print_formula(os, dict, s->get_condition());
    return "div_state(" + os.str() + ")";
  }
} //

std::string sog_tgba::transition_annotation(const spot::tgba_succ_iterator* t) const {
  assert(!t->done());
  const sog_succ_iterator* it = dynamic_cast<const sog_succ_iterator*>(t);
  if (it)
    return it->format_transition();
  else {
    const sog_div_succ_iterator* it = dynamic_cast<const sog_div_succ_iterator*>(t);
    assert(it);
    return it->format_transition();
  }
} //

bdd sog_tgba::all_acceptance_conditions() const {
  return bddfalse;
} //

bdd sog_tgba::neg_acceptance_conditions() const {
  return bddtrue;
} //

bdd sog_tgba::compute_support_conditions(const spot::state* state) const {
  return bddtrue;
} //

bdd sog_tgba::compute_support_variables(const spot::state* state) const {
  return bddtrue;
} //
