#ifndef GAL_VAR_ORDER_HEURISTIC_HH_
#define GAL_VAR_ORDER_HEURISTIC_HH_

#include "GAL.hh"

namespace its {

/** 
 The ordering heuristics.
 force_heuristic is based on FORCE (cf. force.hh)
 lex_heuristic is based on the lexicographical order of variables
 
 \todo add a strategy
 */
labels_t force_heuristic (const GAL * const);
labels_t lex_heuristic (const GAL * const);

} // namespace its

#endif /// ! GAL_VAR_ORDER_HEURISTIC_HH_
