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

#define TRACE

#include <iostream>
#include <sstream>

#include <cassert>

#include "tgba/bddprint.hh"

#include "sogsucciter.hh"
#include "sogstate.hh"

#include "apiterator.hh"

#include  "ITSModel.hh"

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


using namespace its;

namespace sogits {

sog_succ_iterator::sog_succ_iterator(const its::ITSModel& m, const sog_state& s)
  : model(m), from(s), it(APIteratorFactory::create()), div_has_been_visited(true),succstates(from.get_succ()) {
  // set status of iterator to done() initially

  // succstates : initialize Ext = states that serve as seed for successors
  trace << "Built sog_succ_iterator : " << *this << std::endl;
}

  sog_succ_iterator::~sog_succ_iterator() {
    delete current_succ;
  }

void sog_succ_iterator::first() {
  /// position "it" at first of ap bdd set
  // iterate until a non empty succ is found (or end reached)
  for (it.first()  ; ! it.done() ; it.next() ) {
    sog_state s (model, succstates, it.current() );
    if ( s.get_states() != SDD::null ) {
      current_succ = new sog_state(s);
      break;
    }
  }
  if (from.get_div())
    div_has_been_visited = false;
}


void sog_succ_iterator::next() {
  assert(!done());
  if ( ! it.done() ) {
    for (it.next()  ; ! it.done() ; it.next() )
      {
	sog_state s (model, succstates, it.current() );
	if ( s.get_states() != SDD::null ) {
	  current_succ = new sog_state(s);
	  break;
	}
      }
  }
  else
    div_has_been_visited = true;
} //

bool sog_succ_iterator::done() const {
  return  it.done() && div_has_been_visited;
} //

spot::state* sog_succ_iterator::current_state() const {
  assert(!done());
  if (! it.done() ) {
    trace << "FIRING : " << it.current() << std::endl;
    trace << "FROM " << from << std::endl;
    return current_succ;
  }
  else {
    trace << "REACHED DIV STATE" << std::endl << std::endl;
    return new sog_div_state(from.get_condition());
  }
} //

bdd sog_succ_iterator::current_condition() const {
  assert(!done());
  return from.get_condition();
} // 


bdd sog_succ_iterator::current_acceptance_conditions() const {
  assert(!done());
  return bddfalse;
} //

 std::ostream & sog_succ_iterator::print (std::ostream & os) const {
    return (os << "SogSuccIter : from =" << from << std::endl);
  }




sog_div_succ_iterator::sog_div_succ_iterator(const spot::bdd_dict* d, const bdd& c)
  : dict(d), cond(c), div_has_been_visited(true) {
  // => done()
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


} // namespace 

std::ostream & operator << (std::ostream & os, const sogits::sog_succ_iterator &s) {
  return s.print(os);
}


