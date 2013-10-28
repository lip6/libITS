// Copyright (C) 2009, 2010 Laboratoire de Recherche et Developpement de l'Epita
//
// This file is part of Spot, a model checking library->
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version->
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE->  See the GNU General Public
// License for more details->
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING->  If not, write to the Free
// Software Foundation, Inc->, 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA->

#include "sogkripke.hh"
#include "sogstate.hh"

namespace sogits {

sog_kripke::sog_kripke(sogits::sog_tgba* sog_tgba_) :
	sog_tgba_(sog_tgba_) {
}

sog_kripke::~sog_kripke() {
} //


spot::state*
sog_kripke::get_init_state() const {
	return sog_tgba_->get_init_state();
}

spot::tgba_succ_iterator*
sog_kripke::succ_iter(const spot::state* state, const spot::state*, const tgba*) const {
	return sog_tgba_->succ_iter(state);
}

spot::bdd_dict*
sog_kripke::get_dict() const {
	return sog_tgba_->get_dict();
}

bdd sog_kripke::state_condition(const spot::state* state) const {
	const sog_state* s = dynamic_cast<const sog_state*> (state);
	assert(s);
	return s->get_condition();

}

std::string sog_kripke::format_state(const spot::state* s) const {
	return sog_tgba_->format_state(s);
}

}
