#ifndef ENVIRONMENT_HH_
#define ENVIRONMENT_HH_

/**
 We expose here (inline) utilities for environment manipulation.
 \todo C++11 extern templates
 **/

namespace its {

typedef PIntExpression::indexes_t indexes_t;

static std::ostream & operator<< (std::ostream & os, const labels_t & env) {
  if (! env.empty()) {
    for (labels_it it = env.begin() ; /* in loop */ ; /* in loop */ ) {
      os << *it ;
      ++it;
      if (it != env.end())
        os << ", ";
      else
        break;
    }
  }
  return os;
}

static inline int indexOf (Label name, const labels_it & begin, const labels_it & end) {
  int index = 0;
  for (labels_it it = begin;
       it != end;
       ++it,++index ) {
    if (*it == name) {
      return index;
    }
  }
  return -1;
}

// True if "a" is a superset of "b"
static bool supersets ( const labels_t & a, const labels_t & b) {
  labels_it pa = a.begin();
  labels_it pb = b.begin();
  labels_it aend = a.end();
  labels_it bend = b.end();
  
  for ( ; pa !=  aend && pb != bend ; ) {
    if (pa == aend) {
      // b not yet empty !
      return false;
    } else if (pb == bend) {
      // b exhausted, a is a superset
      return true;
    } else if (*pa == *pb) {
      ++pa;
      ++pb;
    } else if (*pa > *pb) {
      // can't matchthis b elt in a
      return false;
    } else {
      // next a elt
      ++pa;
    }
  }
  // a and b were equal
  return true;
}

static inline labels_t sorted_union ( const labels_t & a, const labels_t & b) {
  labels_t unione;
  labels_it pa = a.begin();
  labels_it pb = b.begin();
  labels_it aend = a.end();
  labels_it bend = b.end();
  
  // Quit when both a and b are exhausted
  while ( pa != aend || pb != bend ) {
    // at end of a => push b onto union
    // else if b not empty and is smaller push b onto union
    if ( pa == aend || ( pb != bend && *pb < *pa ) ) {
      unione.push_back(*pb);
      ++pb;
      
      // at end of b => push a onto union
      // else (a is not empty tested above) push a if it is smaller	
    } else if ( pb == bend || *pa < *pb ) {
      unione.push_back(*pa);
      ++pa; 
      
      // Case is only reached if a and b non empty and ( *pa == *pb ) 	
    } else { 
      unione.push_back(*pa);
      ++pa;
      ++pb;
    } 
  }
  return unione;
}

static inline indexes_t reindex ( const labels_t & oldenv, const labels_t & newenv, bool * shouldRewrite ) {
  indexes_t res ( oldenv.size() , 0);
  int count = 0;
  for (labels_it it = oldenv.begin() ; it != oldenv.end() ; ++it, ++count ) {
    int i = indexOf(*it,newenv.begin(), newenv.end());
    if (i == -1) {
      std::cerr << "Error : While reindexing : new environment is not a superset of old one" << std::endl;
      std::cerr << "was looking for  " << *it << " in environment " << newenv << std::endl ;
    }
    
    res [count] = i;
    if (count != i) {
      *shouldRewrite = true ;
    }
  }
  return res;
}

/** shrinks environment to fit expression. If needed cleanup holes and reindex expression. */
template <typename T>
static inline std::pair<T,labels_t> gc (const T & expr, const labels_t env) {
  typedef std::pair<T, labels_t> exprenv_t;
  // prepare new (possibly reduced environment
  size_t oldsize = env.size();
  // for gc : unmarked entries will be cleared
  bool mark [oldsize];
  for (size_t i =0 ; i < oldsize ; ++i)
    mark[i] = false;
  
  // mark as appropriate by traversing the expression
  expr.getSupport(mark);
  
  // build new environment and permutation
  exprenv_t toret (expr, labels_t());
  indexes_t perm;
  size_t pos = 0;
  bool shouldreindex=false;
  
  for (size_t i = 0 ; i < oldsize ; ++i) {
    // label is kept
    if (mark[i]) {
      toret.second.push_back(env[i]);
      if (i != pos)
        shouldreindex = true;
      perm.push_back(pos++);
    } else {
      perm.push_back(-1);
    }
  }
  
  // if should reindex, update expression
  if (shouldreindex)
    toret.first = toret.first.reindexVariables(perm);
  
  return toret;
}

template <typename Expr, typename PExpr>
static inline PExpr normalize (const PExpr & expr, const labels_t & env, const labels_t & unione) {
  // to store whether reindex is necessary
  bool rwl=false;
  // compute possible reindexing of formulas in l.expr
  // eg. reindexl = {"a"}.rw({"a","b"}) = [ 0 , 1 ], and rwl=false since "a" did not move
  indexes_t reindexl = reindex ( env, unione, &rwl);
  
  PExpr x0 = expr;
  // eg. avoid this unecessary reindex
  if (rwl) {
    // eg. would not have any effect anyway
    x0 = x0.reindexVariables(reindexl);
  }
  
  return x0;
}

template <typename Expr, typename PExpr>
static inline PExpr normalize (const Expr & expr, const labels_t & unione) {
  
  return normalize<Expr,PExpr> (expr.getExpr(), expr.getEnv(), unione);
}

} // namespace its

#endif /// !ENVIRONMENT_HH_
