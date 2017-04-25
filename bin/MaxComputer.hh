#ifndef __MAX_COMPUTER__HH__
#define __MAX_COMPUTER__HH__

#include "ddd/SDD.h"
#include "ddd/DDD.h"

class MaxComputer {
public :
  typedef std::pair<long double, long double> stat_t;
private:
  typedef ext_hash_map<GSDD, stat_t> scache_t;
  scache_t scache;
  typedef ext_hash_map<GDDD, stat_t> cache_t;
  cache_t cache;

  // either Max/sum or Min/max
  bool isMaxSum;
  bool firstError;
public : 
  MaxComputer  (bool isMaxSum=true) : scache(),cache(), isMaxSum(isMaxSum),firstError(false) {}
  const stat_t & compute (const GSDD & reach) ;
  const stat_t & compute (const GDDD & reach) ;
  const stat_t & compute (const DataSet * ev) ;

  void printStats (const stat_t & stat, std::ostream & out) const ;
};


#endif
