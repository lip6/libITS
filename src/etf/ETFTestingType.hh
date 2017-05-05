/*
 * ETFTATYPE.hh
 *
 *  Created on: 7 Nov. 2012
 *      Author: Ala Eddine
 */

#ifndef ETFTAType_HH_
#define ETFTAType_HH_

#include "its/TypeBasics.hh"
#include "its/TypeVisitor.hh"


#include <spot/tl/apcollect.hh>
#include <spot/misc/hash.hh>
#include "its/etf/ETFType.hh"
#include <spot/misc/bddlt.hh>

namespace its {
// hold the result of a parse
 class AtomicProposition {
 public :
   int var;
   std::function<bool(int,int)> comp;
   int val;
   AtomicProposition(int var, std::function<bool(int,int)> comp, int val) : var(var),comp(comp),val(val) {}
   AtomicProposition() : var(-1),comp(NULL),val(-1) {}
 };


/** A etf adapter matching the ITS type contract.
 *  Uses (friendly) delegation on Etf class */
class EtfTestingType: public EtfType {

	// Point to the associated dictionnary.
	//spot::bdd_dict* dict;

	// Map the indexes (in dictionnary) of atomic propositions to the
	// corresponding indexes of variables in the states (etfmodel) and their value
	// in the atomic proposition.
	std::map<int, class AtomicProposition> bddvar_to_AtomicProposition;

        std::unordered_map<int, std::vector<class ETFTransition>>
			   changeset_to_transitions;

	vLabel name;

	// a helper function that parses the input ETF file using the primitives of LTSmin and produces the corresponding data structures.
	//void
	//compute_transitions(std::vector<class ETFTransition> & transitions) const;

	void
	compute_transitions_by_changeset(
			std::vector<class ETFTransition> & transitions, bdd changeset) const;

protected:

	// Set an observed atomic proposition : the string corresponding to the AP identifier is then related to the bdd var of index
	AtomicProposition parseAtomicProposition(Label atomicProp) const;
	bdd state_condition(int* state) const;

	//	vector<class ETFTransition>  get_transitions_by_changeset() {
	//		changeset_to_transitions
	//	}

public:

	 EtfTestingType (Label path):EtfType (path)  {} ;


	void setObservedAP(Label atomicProp, int bddvar);


	void
	compute_transitions_by_changesets(
        std::set<bdd, spot::bdd_less_than> allChangesets);

	/** state and transitions representation functions, selected by changeset */
	/** Local transitions by changeset*/
	Transition getLocalsByChangeSet(bdd changeset) const;

/** The state predicate function : string p -> SHom.
	 *  returns a selector homomorphism that selects states verifying the predicate 'p'.
	 *  The syntax of the predicate is left to the concrete type realization.
	 *  The only constraint is that the character '.' is used as a namespace separator
	 *  and should not be used in the concrete predicate syntax.
	 *  Examples : P1.fork = 1 ; P2.P3.think > 0  etc... */
	//Transition getAPredicate(Label predicate) const;


	/// \brief Unregister all the used propositions
	//virtual ~EtfTestingType();
};

} // namespace

#endif /* ETFTATYPE_HH_ */
