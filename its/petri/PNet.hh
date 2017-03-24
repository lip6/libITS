#ifndef __PT_NET_HH__
#define __PT_NET_HH__

#include <string>
#include <utility>
#include <iosfwd>
#include <map>

#include "its/Type.hh"
#include "its/petri/Place.hh"
#include "its/petri/PTransition.hh"
#include "its/petri/Marking.hh"

namespace its {


class PNet : public NamedElement {
public :
	// a type for a set of places
	typedef std::vector<Place> places_t;
	typedef places_t::const_iterator places_it;
	// a type for a set of Petri net transitions
	typedef std::vector<PTransition> trans_t;
	typedef trans_t::const_iterator trans_it;
	// a type to store a set of named markings
	typedef std::map<vLabel,Marking> markings_t;
	typedef markings_t::const_iterator markings_it;

protected :
	// a set of places
	places_t places_;
	// a set of Petri net transitions
	trans_t transitions_;
	// the markings defined for this net
	markings_t markings_;
public :
	// Any Petri net type has a name
  PNet (Label name) : NamedElement(name) { markings_.insert(markings_t::value_type("init",Marking())); };

	// Add a place to this net, returns false if place name already exists.
	bool addPlace (Label pname) ;
	// Add a transition to this net, returns false if this transition already exists
        // The label is used for synchronizations, while name should be unique in the net.
	// Visibility may be public or private.
        bool addTransition (Label tname, Label tlabel, Visibility vis);

	// a type for passing arc descriptions
	typedef std::pair<vLabel,int> ArcVal;
	// Adds an inhibitor, pre or post (depending on at) arc labeled with av.second
	// between a transition "tname" and a place "av.first".
	bool addArc (const ArcVal & av, Label tname, ArcType at);
	// a type for passing hyperarc descriptions, as a set of <place name , arc value>
	typedef std::vector<ArcVal> ArcValSet;
	// Adds a hyperarc to a transition, same principles as addArc.
	bool addHArc (const ArcValSet & avs, Label tname, ArcType at);

	// Marking management : returns true when the state was initialized by this instruction.
	// Adds a single assignment such that : state(place) = value.
	// If the state name was previously unknown it is created and initialized with empty places.
	bool setMarking (Label state, Label place, int value);

	// Accessors

	// places
	places_it places_begin() const { return places_.begin(); }
	places_it places_end() const { return places_.end(); }

	// transitions
	trans_it transitions_begin() const { return transitions_.begin(); }
	trans_it transitions_end() const { return transitions_.end(); }
        trans_it transitions_find (Label lab) const { return findName(lab,transitions_); }

  // markings
  markings_it markings_begin () const { return markings_.begin(); }
  markings_it markings_end () const { return markings_.end(); }
  markings_it markings_find (Label lab) const { return markings_.find(lab); }

	std::ostream & print (std::ostream & os) const ;
};

} // namespace its

std::ostream & operator<< (std::ostream & os, const its::PNet & net);

#endif
