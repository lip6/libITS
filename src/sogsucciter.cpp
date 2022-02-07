// Copyright (C) 2004, 2016 Laboratoire d'Informatique de Paris 6
// (LIP6), d�artement Syst�es R�artis Coop�atifs (SRC),
// Universit�Pierre et Marie Curie.
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



#include <iostream>
#include <sstream>

#include <cassert>

#include <spot/twa/bddprint.hh>

#include "sogsucciter.hh"
#include "sogstate.hh"

#include "apiterator.hh"

#include  "sogIts.hh"


//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


using namespace its;

namespace sogits {

sog_succ_iterator::sog_succ_iterator(const sogIts& m, const sog_state& s)
  : model(m),
    from(s),
    it(APIteratorFactory::create()),
    div_needs_visit(false),
    succstates(from.get_succ()),
    current_succ(NULL) {
  // set status of iterator to done() initially

  // succstates : initialize Ext = states that serve as seed for successors
  trace << "Built sog_succ_iterator : " << *this << std::endl;
}

  sog_succ_iterator::~sog_succ_iterator () {
    delete current_succ;
    delete it;
  }


  void sog_succ_iterator::step() {
    // iterate until a non empty succ is found (or end reached)
    for (  ; ! it->done() ; it->next() ) {
      sog_state s (model, succstates, it->current() );
      if ( s.get_states() != SDD::null ) {
	current_succ = new sog_state(s);
	break;
      }
    }
  }

bool sog_succ_iterator::first() {
  // set whether the div successor exists, i.e. the source agregate contains a circuit
  if (from.get_div())
    div_needs_visit = true;

  /// position "it" at first of ap bdd set
  it->first();
  step();
  return !done();
}


bool sog_succ_iterator::next() {
  assert(!done());
  if (div_needs_visit) {
    div_needs_visit = false;
    return !done();
  }
  // else
  it->next();
  step();
  return !done();
} //

bool sog_succ_iterator::done() const {
  return  it->done() && ! div_needs_visit;
} //

spot::state* sog_succ_iterator::dst() const {
  assert(!done());
  if (! div_needs_visit ) {
    trace << "FIRING : " << it->current() << std::endl;
    trace << "FROM " << from << std::endl;
    return new sog_state(*current_succ);
  } else {
    trace << "REACHED DIV STATE"  << std::endl;
    return new sog_div_state(from.get_condition());
  }
} //

bdd sog_succ_iterator::cond() const {
  assert(!done());
  trace << "Succ iter" << *this << " asked for current cond= " << from.get_condition() << std::endl;
  return from.get_condition();
} //


spot::acc_cond::mark_t sog_succ_iterator::acc() const {
  assert(!done());
  return {};
} //

 std::ostream & sog_succ_iterator::print (std::ostream & os) const {
    return (os << "SogSuccIter : from =" << from << std::endl);
  }




sog_div_succ_iterator::sog_div_succ_iterator(const spot::bdd_dict_ptr& d,
					     const bdd& c)
  : dict(d), cond_(c), div_has_been_visited(true) {
  // => done()
}


bool sog_div_succ_iterator::first() {
  div_has_been_visited = false;
  return true;
}

bool sog_div_succ_iterator::next() {
  assert(!done());
  div_has_been_visited = true;
  return false;
}

bool sog_div_succ_iterator::done() const {
  return div_has_been_visited;
}

spot::state* sog_div_succ_iterator::dst() const {
  assert(!done());
  trace << "FIRING : " << format_transition() << std::endl;
  trace << "FROM a div state" << std::endl << std::endl;
  return new sog_div_state(cond_);
}

bdd sog_div_succ_iterator::cond() const {
  assert(!done());
  return cond_;
}

int sog_div_succ_iterator::current_transition() const {
  assert(!done());
  return -1; // div
}

spot::acc_cond::mark_t sog_div_succ_iterator::acc() const {
  assert(!done());
  return {};
}

std::string sog_div_succ_iterator::format_transition() const {
  assert(!done());
  std::ostringstream os;
  spot::bdd_print_formula(os, dict, cond_);
  return "div(" + os.str() + ")";
}

} // namespace

std::ostream & operator << (std::ostream & os, const sogits::sog_succ_iterator &s) {
  return s.print(os);
}


