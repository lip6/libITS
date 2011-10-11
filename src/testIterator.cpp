#include <iostream>
#include "bdd.h"

#include "apiterator.hh"

using namespace std ;
using namespace sogits ;

int main(int argc, const char ** argv) {
  int nbvar = 0;

  if (argc < 2) {
    std::cerr << "provide number of boolean variables please" << std::endl;
  } else {
    sscanf(argv[1],"%d",&nbvar);
  }

  APIterator::varset_t  var;
  for (int i = 0; i < nbvar; ++i)
    var.push_back(i);

  bdd_init(10000, 10000);
  if (nbvar)
    bdd_setvarnum(nbvar);

  APIteratorFactory::setAPVarSet(var); 
  APIterator * api = APIteratorFactory::create();
  for(api->first(); !api->done(); api->next())
    cout << api->current() << endl;
  delete api;
}
