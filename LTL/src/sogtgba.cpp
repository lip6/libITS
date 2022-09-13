// Copyright (C) 2009, 2016 Laboratoire d'Informatique de Paris 6
// (LIP6), Equipe MoVe , Universite Pierre et Marie Curie.
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

#include <spot/tl/formula.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/twa/bddprint.hh>

#include "sogtgba.hh"
#include "sogsucciter.hh"
#include "sogstate.hh"
#include "bczCSD99.hh"

using its::ITSModel;

//#define TRACE

#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif


namespace sogits {


sog_tgba::sog_tgba(const sogIts & m, const spot::bdd_dict_ptr& dict,
		   sog_product_type type): spot::twa(dict), model(m), type(type) {
} //

sog_tgba::~sog_tgba() {
} //


  spot::state* sog_tgba::create_state (const sogIts & model, const GSDD & m, bdd ap) const {
    if (type == PLAIN_SOG) {
      return new sog_state( model, m, ap );
    } else if (type == BCZ99) {
      return new bcz_state( model, m, ap );
    } else {
      // Definitely a problem here !
      assert(false);
      return nullptr; // for compiler warning
    }
  }
  spot::state* sog_tgba::get_init_state() const {
    its::State m0 = model.getInitialState() ;
    assert(m0 != SDD::null);

    // now determine which AP are true in m0
    APIterator * it = APIteratorFactory::create();
    for (it->first() ; ! it->done() ; it->next() ) {
      its::Transition selector = model.getSelector( it->current() );
      its::State msel = selector(m0);
      if (msel != SDD::null) {
	spot::state * init = create_state(model, m0, it->current());
	trace << "Initial state of tgba verifies :"<< it->current()<< std::endl;
	delete it;
	return init;
      }
    }
    delete it;

    /** should be captured now by emptyAPITerator code */
//     if (APIteratorFactory::empty())
//       return create_state(model, m0, bddtrue);


    // no conjunction of AP is verified by m0 ???
    assert (false);
    return nullptr; // for compiler warning
  }

spot::twa_succ_iterator* sog_tgba::succ_iter(const spot::state* local_state) const {

  if (const sog_state* s = dynamic_cast<const sog_state*>(local_state)) {
    // build a new succ iter :
    //   agregate is built by saturating :
    return new sog_succ_iterator(model , *s);
  } else if (const sog_div_state* s = dynamic_cast<const sog_div_state*>(local_state)) {
    assert(s);
    return new sog_div_succ_iterator(get_dict(), s->get_condition());
  } else if (const bcz_state* s = dynamic_cast<const bcz_state*>(local_state)) {
    return new bcz_succ_iterator(model , *s);
  } else {
    assert(false);
    return nullptr; // for compiler warning
  }
} //

std::string sog_tgba::format_state(const spot::state* state) const {

  if (const sog_state* s = dynamic_cast<const sog_state*>(state)) {
    std::stringstream  ss;
    ss  << *s ;
    return ss.str();
  } else if (const sog_div_state* s = dynamic_cast<const sog_div_state*>(state)) {
    assert(s);
    std::ostringstream os;
    spot::bdd_print_formula(os, get_dict(), s->get_condition());
    return "div_state(" + os.str() + ")";
  } else if (const bcz_state* s = dynamic_cast<const bcz_state*>(state)) {
    std::stringstream  ss;
    ss  << *s ;
    return ss.str();
  } else {
    assert(false);
    return nullptr; // for compiler warning
  }
} //

} // namespace
