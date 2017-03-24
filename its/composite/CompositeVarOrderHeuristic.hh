#ifndef COMPOSITE_VAR_ORDER_HEURISTIC_HH_
#define COMPOSITE_VAR_ORDER_HEURISTIC_HH_

#include "its/Naming.hh"
// grab orderHeuristicType def
#include "its/gal/GALVarOrderHeuristic.hh"

namespace its {


/** 
 The ordering heuristics.
 force_heuristic is based on FORCE (cf. force.hh)
 lex_heuristic is based on the lexicographical order of variables
 
 \todo add a strategy
 */

labels_t force_heuristic (const class Composite & , orderHeuristicType);
labels_t lex_heuristic (const Composite &);

} // namespace its

#endif /// ! COMPOSITE_VAR_ORDER_HEURISTIC_HH_
