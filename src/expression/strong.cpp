/****************************************************************************/
/*								            */
/* This file is part of the PNDDD, Petri Net Data Decision Diagram  package.*/
/*     						                            */
/*     Copyright (C) 2004 Denis Poitrenaud and Yann Thierry-Mieg            */
/*     						                            */
/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU General Public License as published by */
/*     the Free Software Foundation; either version 2 of the License, or    */
/*     (at your option) any later version.                                  */
/*     This program is distributed in the hope that it will be useful,      */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/*     GNU General Public License for more details.                         */
/*     						                            */
/*     You should have received a copy of the GNU General Public License    */
/*     along with this program; if not, write to the Free Software          */
/*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*     						                            */
/****************************************************************************/
 


/* -*- C++ -*- */
// version 0.2

#include <cassert>
#include <iostream>

#include "SDD.h"
#include "DDD.h"

#include "var_set.hpp"
#include "strong.hpp"
#include "expression.hpp"

using namespace std;



// ********************************************************************************
class _SelectCond:public StrongHom {
	PtExpression exp;
	Assignment as;
	Ass_fifo is;


	_SelectCond(const PtExpression& e, const Assignment& a, const Ass_fifo& s ):exp(e), as(a), is(s){
	}

public:
	_SelectCond(const Expression& e):exp(e){
		assert(exp->size()!=0);
	}
	

	GDDD phiOne() const {
		return GDDD::top;
	}

	GHom phi(int vr, int vl) const {
		if (exp->isParam(vr)||exp->isParam_fifo(vr)) {
			Assignment a(as);
			Ass_fifo t(is);
			if(exp->isParam(vr))
				insert(a, vr, vl);
			else
				insert_fifo(t, vr, vl);
			
			if ((exp->size() == a.size()+t.size()) && (exp->isParam(vr) || vl ==-1))
                {
                    if (exp->eval(a,t) != 0 )
					    return GHom(vr, vl) ;
				    else
					    return GHom(DDD::null);
                }
                return GHom(vr, vl, _SelectCond(exp, a ,t)) ;
		}
		return GHom(vr, vl, GHom(this));
	}

	size_t hash() const {
		return 2689*size_t(&(*exp))^asHash(as)^asHash(is);
	}

	bool operator==(const StrongHom &s) const {
		_SelectCond* ps = (_SelectCond*)&s;
		return &(*exp) == &(*(ps->exp)) && as == ps->as && is == ps->is ;
	}
  _GHom * clone () const {  return new _SelectCond(*this); }
};


/*********************************************************************************/
// select the states satisfying (exp!=0)
GShom selectCond(const Expression& exp, const RdPE & R) {
  // We need to analyze the expression to determine target modules of elementary predicates
  // And construct h1 & h2 for AND
  // h1 + h2 for OR etc...
  
  // treat degenerated constant case
  if (exp.size()==0) {
    Assignment a;
    Ass_fifo at;
    if (exp.eval(a,at)!=0)
      return GShom::id;
    else 
      return GShom(SDD::null);
  } else {
    // Let's get serious
    
    // An expression contains 
    // in "is" the list of touched place indexes
    // in "fifo" the list of touched fifos
    // find target modules
    set<int> targetMod;
    for (IntSet::iterator it = exp.is.begin(); it != exp.is.end() ; it++) 
      targetMod.insert(R.getmodule(*it));
    for (IntSet::iterator it = exp.fifo.begin(); it != exp.fifo.end() ; it++) 
      targetMod.insert(R.getmodule(*it));
    
    // See if we can just target a single module
    if (targetMod.size() == 1) {
     
      int target = * targetMod.begin();
//      cerr << "This expression targets a single module ("<<target <<")"<<endl;

      Tree modexp = exp.root->clone();
      modexp->convToModuleRelativeIndex (R);
      return localApply(Hom( _SelectCond(Expression(modexp))),target);
    } else {
      // Recurse until expression is single module targetted
      if (exp.root->t == OR) {
//	cerr << "Found an OR" <<endl;
	return selectCond(Expression (exp.root->fg),R) + selectCond(Expression (exp.root->fd),R);
      } else if (exp.root->t == AND) {
//	cerr << "Found an AND" <<endl;
	return selectCond(Expression (exp.root->fg->clone()),R) & selectCond(Expression (exp.root->fd->clone()),R);
      } else {
	cerr << "Sorry your expression avaluation test is not fully implemented yet." << endl;
	cerr << "Will return 0 for this part of expression" <<endl;
	return GSDD::null;
      }
      
    }
  }
}


