#ifndef __EXPRESSION_HOM_HH__
#define __EXPRESSION_HOM_HH__

#include "Hom.h"
#include "gal/IntExpression.hh"
#include "gal/BoolExpression.hh"
#include "gal/GALOrder.hh"

namespace its {

/// Assigns the value of expr to variable var.
GHom assignExpr (const IntExpression & var, const IntExpression & expr, const GalOrder * vo);

/// Creates a predicate to select paths that verify the expression e.
GHom predicate (const BoolExpression & e, const GalOrder * vo);

#ifdef HASH_STAT
void query_stats ();
#endif // HASH_STAT
}

#endif
