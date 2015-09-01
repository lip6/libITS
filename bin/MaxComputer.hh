#ifndef __MAX_COMPUTER__HH__
#define __MAX_COMPUTER__HH__

#include "SDD.h"

class MaxComputer {
public :
  typedef std::pair<long double, long double> stat_t;
private:
  typedef ext_hash_map<GSDD, stat_t> scache_t;
  scache_t scache;
  typedef ext_hash_map<GDDD, stat_t> cache_t;
  cache_t cache;


public : 
  const stat_t & compute (const GSDD & reach) ;

  void printStats (std::ostream & out) const ;
};


#endif
