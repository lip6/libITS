#ifndef GAL_VAR_ORDER_HEURISTIC_HH_
#define GAL_VAR_ORDER_HEURISTIC_HH_

#include "its/Naming.hh"

namespace its {

// DEFAULT: do not follow the 'call' statements
// FOLLOW: follows 'call' statements
enum orderHeuristicType
{
  DEFAULT,        // dont follow 'call' labels
  FOLLOW,         // follow 'call' labels
  FOLLOW_HALF,    // follow 'call' labels with half weight
  FOLLOW_DOUBLE,  // follow 'call' labels with double weight
  FOLLOW_SQ,      // FOLLOW + square cost
  FOLLOW_DYN,     // follow 'call' labels with weight related to size
  FOLLOW_DYN_SQ,  // FOLLOW_DYN + square cost
  FOLLOW_FDYN,    // follow 'call' labels, all weights are related to size
  FOLLOW_FDYN_SQ, // FOLLOW_FDYN + square cost
  SATUR,          // FOLLOW + saturation-friendly shifts
  LEXICO          // historical lexicographical heuristic
};

/** 
 The ordering heuristics.
 force_heuristic is based on FORCE (cf. force.hh)
 lex_heuristic is based on the lexicographical order of variables
 
 \todo add a strategy
 */

labels_t force_heuristic (const class GAL * const, orderHeuristicType);
labels_t lex_heuristic (const class GAL * const);

/** a helper to sort lexico numeric */
bool less_var ( Label a , Label b );

} // namespace its

#endif /// ! GAL_VAR_ORDER_HEURISTIC_HH_
