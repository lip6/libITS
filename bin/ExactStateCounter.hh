#ifndef __EXACT_COMPUTER__HH__
#define __EXACT_COMPUTER__HH__

#include "SDD.h"
#include "DDD.h"

#include <gmpxx.h>

class ExactStateCounter {
public :
  typedef mpz_class stat_t;
private:
  typedef ext_hash_map<GSDD, stat_t> scache_t;
  scache_t scache;
  typedef ext_hash_map<GDDD, stat_t> cache_t;
  cache_t cache;

  bool firstError;
public : 
  ExactStateCounter  () : scache(),cache(),firstError(false) {}
  const stat_t & compute (const GSDD & reach) ;
  const stat_t & compute (const GDDD & reach) ;
  const stat_t & compute (const DataSet * ev) ;

  void printStats (const stat_t & stat, std::ostream & out) const ;
};


#endif
