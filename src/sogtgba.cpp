// Copyright (C) 2009  Laboratoire d'Informatique de Paris 6 (LIP6),
// Equipe MoVe , Universite Pierre et Marie Curie.
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

using its::ITSModel;

//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


namespace sogits {


sog_tgba::sog_tgba(const bddITSModelAdapter & m, 
		   spot::bdd_dict* dict): model(m),dict(dict) {
} //

sog_tgba::~sog_tgba() {
  if (dict)
    dict->unregister_all_my_variables(this);
} //

spot::state* sog_tgba::get_init_state() const {
  its::State m0 = model.getModel().getInitialState() ;
  assert(m0 != SDD::null);
  // now determine which AP are true in m0
  APIterator it = APIteratorFactory::create();
  for (it.first() ; ! it.done() ; it.next() ) {
    its::Transition selector = model.getSelector( it.current() );
    its::State msel = selector(m0);
    if (msel != SDD::null) {
      sog_state * init = new sog_state( model, m0, it.current() );
      trace << "Initial state of tgba :" << *init << "verifies :"<< it.current()<< std::endl; 
      return init;
    }
  }
 
 // no conjunction of AP is verified by m0 ???
  assert (false);
  // for compiler happiness
  return NULL;
}

spot::tgba_succ_iterator* sog_tgba::succ_iter (const spot::state* local_state,
                                               const spot::state*, const spot::tgba*) const {
  const sog_state* s = dynamic_cast<const sog_state*>(local_state);
  if (s) {
    // build a new succ iter :
    //   agregate is built by saturating : 
    return new sog_succ_iterator(model , *s);
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
    std::stringstream  ss;
    ss  << *s ;    
    return ss.str();
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
    return "A transition" ; //it->format_transition();
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


} // namespace


