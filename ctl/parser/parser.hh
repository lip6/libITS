#ifndef __CTL_PARSER_PARSER_HH__
#define __CTL_PARSER_PARSER_HH__

#include <string>

#include "ctl/formula.hh"
#include "ctl/property.hh"

namespace ctl {

////////////////////////////////////////////////////////////////////

formulae
parse_file( const std::string& , const property_factory&);

////////////////////////////////////////////////////////////////////

} // namespace ctl

#endif // __CTL_PARSER_PARSER_HH__
