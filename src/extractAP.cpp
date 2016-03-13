

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <cstring>

#include <bddx>
#include <spot/tl/parse.hh>
#include <spot/tl/apcollect.hh>

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
