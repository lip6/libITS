#ifndef __EXPRESSION_HOM_HH__
#define __EXPRESSION_HOM_HH__

#include "Hom.h"
#include "IntExpression.hh"
#include "BoolExpression.hh"
#include "Ordering.hh"

namespace its {

/// Assigns the value of expr to variable var.
GHom assignExpr (const IntExpression & var, const IntExpression & expr, const VarOrder * vo);

/// Creates a predicate to select paths that verify the expression e.
GHom predicate (const BoolExpression & e, const VarOrder * vo);

#ifdef HASH_STAT
void query_stats ();
#endif // HASH_STAT
}

#endif
