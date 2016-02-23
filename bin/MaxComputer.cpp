#include "MaxComputer.hh"
#include <iostream>

static const MaxComputer::stat_t zero = MaxComputer::stat_t(0,0);

const MaxComputer::stat_t & MaxComputer::compute (const GSDD & d) {
  if (d == GSDD::one || d == GSDD::top || d == GSDD::null) {
    return zero;
  } 
  scache_t::accessor access;  
  scache.find(access,d);

  if( access.empty() ) {
    // miss
    stat_t res = stat_t(0,0);
    for(GSDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
      const stat_t & childStat = compute (gi->second);
      const stat_t & edgeStat = compute (gi->first);

      res.first = std::max(res.first, std::max(childStat.first, edgeStat.first));
      res.second = std::max(res.second, childStat.second + edgeStat.second);
    }
    scache.insert(access,d);
    access->second = res;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}


const MaxComputer::stat_t & MaxComputer::compute (const GDDD & d) {
  if (d == GDDD::one || d == GDDD::top || d == GDDD::null) {
    return zero;
  } 
  cache_t::accessor access;  
  cache.find(access,d);

  if( access.empty() ) {
    // miss
    stat_t res = stat_t(0,0);
    for(GDDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
      const stat_t & childStat = compute (gi->second);
      
      res.first = std::max(res.first, childStat.first > gi->first ? childStat.first : gi->first );
      res.second = std::max(res.second, childStat.second + gi->first );
    }
    cache.insert(access,d);
    access->second = res;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}

static MaxComputer::stat_t zero_stat = MaxComputer::stat_t(0,0);

const MaxComputer::stat_t & MaxComputer::compute (const DataSet* g)
{
    // Used to work for referenced DDD
    if (typeid(*g) == typeid(GSDD) ) {
      return compute ( GSDD ((SDD &) *g) );
    } else if (typeid(*g) == typeid(DDD)) {
      return compute ( GDDD ((DDD &) *g) );
      //    } else if (typeid(*g) == typeid(IntDataSet)) {
      // nothing, no nodes for this implem
      //return stat_t();
    } else {
      if (firstError) {
        std::cerr << "Warning : unknown referenced dataset type on arc, node count is inacurate"<<std::endl;
        std::cerr << "Read type :" << typeid(*g).name() <<std::endl ;
	firstError = false;
      }
      return zero_stat;
    }
}

void MaxComputer::printStats (const MaxComputer::stat_t & stat, std::ostream & out) const  {
  out << "Max variable value : " << stat.first << "\nMaximum sum along a path : " << stat.second << std::endl;
}
