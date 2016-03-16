#include "ExactStateCounter.hh"
#include <iostream>

const ExactStateCounter::stat_t & one_stat (){
  static ExactStateCounter::stat_t one_stat = 1 ;
  return one_stat;
}

const ExactStateCounter::stat_t & zero_stat (){
  static ExactStateCounter::stat_t zero_stat = 0 ;
  return zero_stat;
}



const ExactStateCounter::stat_t & ExactStateCounter::compute (const GSDD & d) {
  if (d == GSDD::one || d == GSDD::top) {
    return one_stat();
  } else if (d == GSDD::null) {
    return zero_stat();
  }
  scache_t::accessor access;  
  scache.find(access,d);

  if( access.empty() ) {
    // miss
    stat_t res = 0;
    for(GSDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
      stat_t childStat = compute (gi->second);
      const stat_t & edgeStat = compute (gi->first);
      stat_t prod = childStat * edgeStat ;
      res = res  + prod ;
    }
    scache.insert(access,d);
    access->second = res ;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}

const ExactStateCounter::stat_t & ExactStateCounter::compute (const GDDD & d) {
  if (d == GDDD::one || d == GDDD::top) {
    return one_stat();
  } else if (d == GDDD::null) {
    return zero_stat();
  }
  cache_t::accessor access;  
  cache.find(access,d);

  if( access.empty() ) {
    // miss
    stat_t res = 0;
    for(GDDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
      const stat_t & childStat = compute (gi->second);
      res += childStat;
    }
    cache.insert(access,d);
    access->second = res;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}


const ExactStateCounter::stat_t & ExactStateCounter::compute (const DataSet* g)
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
      return zero_stat();
    }
}

void ExactStateCounter::printStats (const ExactStateCounter::stat_t & stat, std::ostream & out) const  {
	char * pp;
	gmp_asprintf(&pp,"%Zd",stat.get_mpz_t()); 
	out << "Exact state count : " << *pp << std::endl;
	free (pp);
}
