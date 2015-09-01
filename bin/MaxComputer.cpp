#include "MaxComputer.hh"

static const stat_t zero = stat_t(0,0);

const stat_t & MaxComputer::compute (const GSDD & d) {
  if (d == GSDD::one || d == GSDD::top || d == GSDD::null) {
    return zero;
  } 
  cache_t::accessor access;  
  cache.find(access,g);

  if( access.empty() ) {
    // miss
    stat_t res = stat_t(0,0);
    for(GSDD::const_iterator gi=g.begin();gi!=g.end();++gi) {
      const stat_t & childStat = compute (gi->second);
      if (childS
      res+=(gi->first->set_size())*nbStates(gi->second)+val;
    }
    cache.insert(access,g);
    access->second = res;
    return res;
  } else {
    // hit
    return access->second;
  }
}
