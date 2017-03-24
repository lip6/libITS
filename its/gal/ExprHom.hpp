#ifndef __EXPRESSION_HOM_HH__
#define __EXPRESSION_HOM_HH__

#include "ddd/Hom.h"
#include "its/gal/IntExpression.hh"
#include "its/gal/BoolExpression.hh"
#include "its/gal/GALOrder.hh"

namespace its {

/// Assigns the value of expr to variable var.
GHom assignExpr (const IntExpression & var, const IntExpression & expr, const GalOrder * vo, bool isIncrement=false);

/// Increment value of var by amount.
GHom incrExpr (const IntExpression & var, const IntExpression & expr, const GalOrder * vo);

/// Synchronous assignments
GHom syncAssignExpr (const std::vector<std::pair<IntExpression, IntExpression>> &, const GalOrder * vo);

/// Creates a predicate to select paths that verify the expression e.
/// NB : e should be simplified, e.g. through e.eval()
GHom predicate (const BoolExpression & e, const GalOrder * vo);

#ifdef HASH_STAT
void query_stats ();
#endif // HASH_STAT
}

#endif
