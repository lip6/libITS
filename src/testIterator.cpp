#include <iostream>
#include "bdd.h"

#include "apiterator.hh"

using namespace std ;
using namespace sogits ;

int main(void) {
  const int nbvar = 5;
  APIterator::varset_t  var;
  for (int i = 0; i < nbvar; ++i)
    var.push_back(i);

  bdd_init(10000, 10000);
  bdd_setvarnum(nbvar);

  APIteratorFactory::setAPVarSet(var); 
  APIterator api = APIteratorFactory::create();
  for(api.first(); !api.done(); api.next())
    cout << api.current() << endl;
}
