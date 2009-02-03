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

//#define TRACE

#include <iostream>
#include <sstream>
#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

#include <cassert>

#include "tgba/bddprint.hh"

#include "sogsucciter.hh"
#include "sogstate.hh"
#include "petrinet.hh"

#include "RdPBDD.h"

sog_succ_iterator::sog_succ_iterator(const RdPBDD& pn, const sog_state& s, const bdd& c)
  : pnbdd(pn), from(s.get_marking()), div(s.get_div()), cond(c), 
    it(pnbdd.get_obtr().end()), div_has_been_visited(true) {
  // => done()
}

sog_succ_iterator::sog_succ_iterator(const sog_succ_iterator& s): pnbdd(s.pnbdd), from(s.from) {
  assert(false);
} //

sog_succ_iterator& sog_succ_iterator::operator=(const sog_succ_iterator& s) {
  assert(false);
  return *this;
} //

sog_succ_iterator::~sog_succ_iterator() {
} //

void sog_succ_iterator::first() {
  it = pnbdd.get_obtr().begin();
  while (it != pnbdd.get_obtr().end() && !pnbdd.firable(from, *it))
    ++it;
  if (div)
    div_has_been_visited = false;
} //

void sog_succ_iterator::next() {
  assert(!done());
  if (it != pnbdd.get_obtr().end()) {
    ++it;
    while (it != pnbdd.get_obtr().end() && !pnbdd.firable(from, *it))
      ++it;
  }
  else
    div_has_been_visited = true;
} //

bool sog_succ_iterator::done() const {
  return (it == pnbdd.get_obtr().end()) && div_has_been_visited;
} //

spot::state* sog_succ_iterator::current_state() const {
  assert(!done());
  trace << "FIRING : " << format_transition() << std::endl;
  trace << "FROM " << pnbdd.format_marking(from) << std::endl;
  if (it != pnbdd.get_obtr().end()) {
    bool current_dead, current_div;
    bdd m = pnbdd.successor(from, *it, current_dead, current_div);
    assert((m != bddfalse) || current_dead); // (m == bddfalse) => current_dead
    trace << "REACHED " << pnbdd.format_marking(m) << std::endl << std::endl;
    return new sog_state(m, current_div | current_dead);
  }
  else {
    trace << "REACHED DIV STATE" << std::endl << std::endl;
    return new sog_div_state(cond);
  }
} //

bdd sog_succ_iterator::current_condition() const {
  assert(!done());
  return cond;
} //

int sog_succ_iterator::current_transition() const {
  assert(!done());
  if (it != pnbdd.get_obtr().end())
    return *it;
  return -1; // div
}

bdd sog_succ_iterator::current_acceptance_conditions() const {
  assert(!done());
  return bddfalse;
} //

std::string sog_succ_iterator::format_transition() const {
  assert(!done());
  if (it != pnbdd.get_obtr().end())
    return pnbdd.get_pn().get_transition_name(*it);
  return "div";
} //

sog_div_succ_iterator::sog_div_succ_iterator(const spot::bdd_dict* d, const bdd& c)
  : dict(d), cond(c), div_has_been_visited(true) {
  // => done()
}

sog_div_succ_iterator::~sog_div_succ_iterator() {
}

void sog_div_succ_iterator::first() {
  div_has_been_visited = false;
}

void sog_div_succ_iterator::next() {
  assert(!done());
  div_has_been_visited = true;
}

bool sog_div_succ_iterator::done() const {
  return  div_has_been_visited;
}

spot::state* sog_div_succ_iterator::current_state() const {
  assert(!done());
  trace << "FIRING : " << format_transition() << std::endl;
  trace << "FROM a div state" << std::endl << std::endl;
  return new sog_div_state(cond);
}

bdd sog_div_succ_iterator::current_condition() const {
  assert(!done());
  return cond;
}

int sog_div_succ_iterator::current_transition() const {
  assert(!done());
  return -1; // div
}

bdd sog_div_succ_iterator::current_acceptance_conditions() const {
  assert(!done());
  return bddfalse;
}

std::string sog_div_succ_iterator::format_transition() const {
  assert(!done());
  std::ostringstream os;
  spot::bdd_print_formula(os, dict, cond);
  return "div(" + os.str() + ")";
}

sog_div_succ_iterator::sog_div_succ_iterator(const sog_div_succ_iterator& s) {
  assert(false);
}

sog_div_succ_iterator& sog_div_succ_iterator::operator=(const sog_div_succ_iterator& s) {
  assert(false);
  return *this;
}
