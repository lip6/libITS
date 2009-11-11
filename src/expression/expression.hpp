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
// version 0.3

#ifndef _EXPRESSION_
#define _EXPRESSION_

#include <cassert>
#include <iostream>
#include <map>
using namespace std;

#include "Word.hpp"
#include "var_set.hpp"

//-------------------------------------------------------

typedef map<int, int> Assignment;

inline void insert(Assignment &as, int var, int val) { 
	as[var] = val; 
}

inline int get(const Assignment &as, int var) {
	Assignment::const_iterator i = as.find(var);
	assert(i!=as.end());
	return i->second;
}
inline size_t asHash(const Assignment& as) {  
	size_t r = 1;
	for (Assignment::const_iterator i=as.begin();i!=as.end();i++)
		r^=i->first^i->second;
	return r*2999;
}

typedef enum {CONST, VAR, PLUS, MINUS, NEG, MULT,
               DIV,MOD,EQUAL,LESSER, GREATER, NOT,AND,OR ,HEAD ,
              BELONGS ,QUEUE , SIZE ,WORD ,FIFO, CONCAT}Type;

 /*************************************************************************************/
//________________________Class Vertex_____________________________
class Vertex;
class Expression;

typedef Vertex* Tree;

class Vertex{
  friend class Expression;
  friend class GShom selectCond(const Expression& exp, const class RdPE & R);
  friend Expression operator+(const Expression& og, const Expression& od);
  friend Expression operator-(const Expression& og, const Expression& od);
  friend Expression operator-(const Expression& o);
  friend Expression operator*(const Expression& og, const Expression& od);
  friend Expression operator/(const Expression& og, const Expression& od);
  friend Expression operator%(const Expression& og, const Expression& od);
  friend Expression operator==(const Expression& og, const Expression& od);
  friend Expression operator<(const Expression& og, const Expression& od);
  friend Expression operator>(const Expression& og, const Expression& od);
  friend Expression operator!(const Expression& o);
  friend Expression operator&&(const Expression& og, const Expression& od);
  friend Expression operator&(const Expression& og, const Expression& od);
  friend Expression operator||(const Expression& og, const Expression& od);
  friend Expression head(const Expression& og, const Expression& od);
  friend Expression queue(const Expression& og, const Expression& od);
  friend Expression belongs(const Expression& og, const Expression& od);
  void findTargets (pair<IntSet,IntSet> &);
  void convToModuleRelativeIndex (const class RdPE & R);
 private:
  Vertex(Type ty, int va, Tree g=NULL, Tree d=NULL);
  Vertex(Type ty, Word va, Tree g=NULL, Tree d=NULL);
  int eval(const Assignment& as, const Ass_fifo& at) const;
  Word eval(const Ass_fifo& at, Tree tree)const;
  Tree clone() const;
  ~Vertex();
  bool operator==(const Vertex& a) const;
  int hash() const;
  Type t;
  int v;
  Word word;
  Tree fg, fd;
};

/***********************************************************************************/
//_______________________Class Expression_________________________
class Expression {
 public:	
  // Constructors
  Expression();
  Expression(int i, Type t = CONST);
  Expression(Word word, Type t = WORD);
  Expression(const Expression& e);	
  Expression (Tree root);
  Expression& operator=(const Expression& e);
  ~Expression();
  bool isEqual(const Expression &e) const;
  size_t hash() const;


  friend class GShom selectCond(const Expression& exp, const class RdPE & R);			
  // operators
  friend Expression operator+(const Expression& og, const Expression& od);	
  friend Expression operator-(const Expression& og, const Expression& od);
  friend Expression operator-(const Expression& o);
  friend Expression operator*(const Expression& og, const Expression& od);
  friend Expression operator/(const Expression& og, const Expression& od);
  friend Expression operator%(const Expression& og, const Expression& od);
  friend Expression operator==(const Expression& og, const Expression& od);
  friend Expression operator!(const Expression& o);
  friend Expression operator<(const Expression& og, const Expression& od);
  friend Expression operator>(const Expression& og, const Expression& od);
  friend Expression operator&&(const Expression& og, const Expression& od);
  friend Expression operator||(const Expression& og, const Expression& od);
  friend Expression operator&(const Expression& og, const Expression& od);
  friend Expression head(const Expression& og, const Expression& od);
  friend Expression queue(const Expression& og, const Expression& od);
  friend Expression belongs(const Expression& og, const Expression& od);
  bool isParam(int var) const;
  bool isParam_fifo(int var)const;	
  unsigned int size() const;
  int eval(const Assignment &as, const Ass_fifo &at) const;

 private:
  
  Expression gen_un_op(Type ty) const;
  Expression gen_bin_op(const Expression& od, Type ty) const;
  IntSet is;
  IntSet fifo;
  Tree root;
};

/***********************************************************************************/
//_______________________Class PtExpression_________________________
class PtExpression {
 public :
  PtExpression(const Expression& e) {
    p = new PtExp(e);
  };

  PtExpression(const PtExpression& r) {
    p = r.p;
    p->nbref++;
  };

  PtExpression& operator=(const PtExpression& r) {
    if (this != &r && p != r.p) {
      p->nbref--;
      if (p->nbref==0)
	delete p;	
      p = r.p;
      p->nbref++;
    }
    return *this;
  };

  ~PtExpression() {
    p->nbref--;
    if (p->nbref==0)
      delete p;
  };

  const Expression* operator->() const {
    return &(p->e);
  };

  const Expression& operator*() const {
    return p->e;
  };

 private:
  struct PtExp {
    PtExp(const Expression& ex) : nbref(1), e(ex) {}
    int nbref;
    Expression e;
  };
  PtExp* p;
};



#endif
