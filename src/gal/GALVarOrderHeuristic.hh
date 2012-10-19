#ifndef GAL_VAR_ORDER_HEURISTIC_HH_
#define GAL_VAR_ORDER_HEURISTIC_HH_

#include "GAL.hh"

namespace its {

labels_t force_heuristic (const GAL * const, bool, bool, bool);
labels_t lex_heuristic (const GAL * const);

} // namespace its

#endif /// ! GAL_VAR_ORDER_HEURISTIC_HH_
