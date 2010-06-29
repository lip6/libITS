#include <iostream>
#include "PNet.hh"
#include <utility>

namespace its {

bool PNet::addPlace (Label pname) {
  if ( findName(pname, places_)  == places_.end() ) {
    places_.push_back(Place(pname));
    // Add a "transition" to test for atomic property : place is marked
    vLabel tname = pname;
    addTransition(tname, tname, PUBLIC);
    addArc(ArcVal(pname,1),tname,TEST);
    return true;
  }
  return false;
}


  bool PNet::addTransition (Label tname, Label tlabel, Visibility vis) {
  if ( findName(tname,transitions_) == transitions_.end() ) {
    transitions_.push_back(PTransition(tname,tlabel,vis));
      return true;
  }
  return false;
}



static void addArcErrorHelper (const std::string & msg, ArcType at, Label pname, int valuation, Label tname) {
  std::cerr << msg << " when attempting to addArc( <"
       << pname << ","
       << valuation << ">, "
       << tname << ","
       << toString(at) 
       << ")" << std::endl;
}

// Adds an inhibitor, pre or post arc to a transition
bool PNet::addArc (const ArcVal & av, Label tname, ArcType at) {
  places_it p = findName(av.first,places_);
  // cast to non const status.
  trans_it t = findName(tname,transitions_);
  if (p == places_.end() || t == transitions_.end()) {
    std::string msg = "Unknown ";
    if (p== places_.end())
      msg += " place " + av.first + " ";
    else
      msg += " transition " + tname + " ";
    addArcErrorHelper(msg,at,av.first,av.second,tname);
    return false;
  }
  // build arc and add to transition
  Arc a = Arc (at, av.first, av.second);
  // const introduced because of findName signature, ok to remove.
  const_cast<PTransition &>(*t).addArc(a);

  return true;
}

// Adds an inhibitor hyperarc to a transition
bool PNet::addHArc (const ArcValSet & avs, Label tname, ArcType at) {

	/** Consistency checks */
	/** non empty arc list */
	if ( avs.empty() ) {
	  std::cerr << "empty places list passed to addHArc(" << tname << "," << toString(at) << ")"<< std::endl;
	  return false;
	}
	/** transition existence */
	trans_it t = findName(tname,transitions_);
	if (t == transitions_.end()) {
		std::string msg = "Unknown ";
		msg += " transition " + tname + " ";
		ArcValSet::const_iterator pv = avs.begin();
		addArcErrorHelper(msg,at,"["+pv->first+"...]",pv->second,tname);
		return false;
	}
	/** places existence */
	for ( ArcValSet::const_iterator avit = avs.begin() ; avit != avs.end() ; ++avit ) {
		places_it p = findName(avit->first,places_);
		if ( p == places_.end() ) {
		      std::string msg = "Unknown ";
		      msg += " place " + avit->first + " ";
		      addArcErrorHelper(msg,at,"[.."+avit->first+"..]",avit->second,tname);
		      return false;
		}
	}
	/** all set, create and add hyper arc to transition */
	{
		ArcValSet::const_iterator avit = avs.begin() ;
		// begin is not past the end, already tested above.
		Arc a (at, avit->first, avit->second);
		for ( ++avit ; avit != avs.end() ; ++avit ) {
			a.addPlace(avit->first, avit->second);
		}
		// const introduced because of findName signature, ok to remove.
		const_cast<PTransition &>(*t).addArc(a);
	}
	return true;
}

// Marking management : returns true when the state was initialized by this instruction.
bool PNet::setMarking (Label state, Label place, int value){
	bool ret = false;
	markings_t::iterator it = markings_.find(state);
	if (it == markings_.end()) {
		// create a new state
		Marking m;
		it = markings_.insert (markings_t::value_type(state,m)).first;
		ret = true;
	}
	// update the marking
	it->second.setMarking(place,value);
	return ret;
}

std::ostream &  PNet::print (std::ostream & os) const {
	  os << "TPNet " << getName() << " {\n\n";

	  os << "    // places definition\n" ;
	  for (places_it pit = places_.begin() ; pit != places_.end() ; ++pit ) {
		  os << (*pit);
	  }

	  os << "    // states definition\n" ;
	  for (markings_t::const_iterator mit=markings_.begin();
	       mit != markings_.end();
	       ++mit ) {
	    os << "    state " << mit->first ;
	    mit->second.print(os);
	    os << std::endl;
	  }

	  os << "\n    // transitions definition\n" ;
	  for (trans_it tit = transitions_.begin(); tit != transitions_.end() ; ++tit) {
	    os << *tit ;
	  }

	  os << "}\n";
	  return os;
}

} // namespace

std::ostream & operator<< (std::ostream & os, const its::PNet & net) {
	net.print(os);
	return os;
}

