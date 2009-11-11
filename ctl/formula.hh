#ifndef __CTL_FORMULA_HH__
#define __CTL_FORMULA_HH__

#include <memory>
#include <vector>

#include <boost/variant.hpp>

#include "ctl/constants.hh"
#include "ctl/property.hh"
#include "ctl/logical_fwd.hh"
#include "ctl/temporal_fwd.hh"

namespace ctl {

////////////////////////////////////////////////////////////////////

/// Define a formula. In fact, it is the AST of the formula.
/// To parse it, use boost::variant visitors.
/// @warning True must be the first, as boost variant
/// default constructs with its first template paramater
typedef boost::variant
        < True
        , False
        , const property*
        , boost::recursive_wrapper< logical_operator  >
        , boost::recursive_wrapper< temporal_operator >
        >
        formula;

////////////////////////////////////////////////////////////////////

/// Define a set of formulae.
typedef std::vector< formula >
        formulae;

////////////////////////////////////////////////////////////////////

} // namespace ctl

#include "ctl/logical.hh"
#include "ctl/temporal.hh"

////////////////////////////////////////////////////////////////////

#endif // __CTL_FORMULA_HH__
