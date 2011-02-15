

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <cstring>

#include "bdd.h"
#include "ltlparse/public.hh"
#include "ltlvisit/destroy.hh"
#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/environment.hh"
#include "ltlparse/public.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaalgos/gtec/gtec.hh"
#include "tgbaalgos/magic.hh"
#include "tgbaalgos/stats.hh"
#include "tgbaalgos/emptiness.hh"
#include "tgbaalgos/dotty.hh"
#include "tgba/tgbatba.hh"

#include "tgbaalgos/sccfilter.hh"


using std::cerr;
using std::string;
using std::endl;


int main (int argc, char ** argv) {
  const char * ltl_string = argv[1];

    // Initialize spot
  spot::ltl::parse_error_list pel;

  spot::ltl::formula* f = spot::ltl::parse(ltl_string, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltl_string, pel)) {
    f->destroy();
    return 1;
  }

  spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(f);

  if (sap) {
    spot::ltl::atomic_prop_set::iterator it;
    for(it = sap->begin(); it != sap->end(); ++it) {
      std::cout << (*it)->name() << std::endl;
    }
  }

  return 0;
}
