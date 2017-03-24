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
 


/****************************************************************************************
*  created by   : Mr Denis POITRENAUD and Cherif Reda HASSAINE
*  created date : 19/05/2003
*  subject      : class  Expression
*  version      : 0.2
****************************************************************************************/

#include <cassert>
#include <iostream>
#include <algorithm>
using namespace std;
#include "its/expression/var_set.hpp"
#include "its/expression/Word.hpp"
#include "its/expression/expression.hpp"
#include "its/parser_RdPE/RdPE.h"

/**********************************************************************************************/
//___________________________Class Vertex_________________________________
Vertex::Vertex(Type ty, int va, Tree g, Tree d):t(ty),v(va),fg(g),fd(d) { }
Vertex::Vertex(Type ty, Word va, Tree g, Tree d):t(ty),word(va),fg(g),fd(d) {  }


void Vertex::convToModuleRelativeIndex (const class RdPE & R) {
  switch (t) {
  case VAR:case SIZE: case FIFO:
    v = R.getmodindex(v);
    return;
  case PLUS :  case MULT :   case MINUS :   case DIV :
  case MOD :   case BELONGS :
  case LESSER :  case GREATER :   case EQUAL : 
  case AND :  case OR : 
  case QUEUE:  case HEAD :
    fg->convToModuleRelativeIndex (R);
    fd->convToModuleRelativeIndex (R);
    return ;
  case NEG : case NOT : 
    fg->convToModuleRelativeIndex (R);
    return ;
  case WORD:  case CONST: case CONCAT:
    return ;
    
  }
  return ; 
}

void Vertex::findTargets (pair<IntSet,IntSet> &targets) {
  switch (t) {
  case VAR:
    targets.first.insert(v);
    return;
  case SIZE: case FIFO:
    targets.second.insert(v);
    return ;
  case PLUS :  case MULT :   case MINUS :   case DIV :
  case MOD :   case BELONGS :
  case LESSER :  case GREATER :   case EQUAL : 
  case AND :  case OR : 
  case QUEUE:  case HEAD :
    fg->findTargets (targets);
    fd->findTargets (targets);
    return ;
  case NEG : case NOT : 
    fg->findTargets (targets);
    return ;
  case WORD:  case CONST: case CONCAT:
    return ;
    
  }
  return ; 

}

int Vertex::eval(const Assignment& as, const Ass_fifo& at) const {
	switch (t) {
	case CONST:
		return v;
	case VAR:
		return get(as, v);
	case PLUS : 
                 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

		     return fg->eval(as,at) + fd->eval(as,at);
	case MINUS : 
                  assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD && 
		  fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

        	     return fg->eval(as,at) - fd->eval(as,at);
	case NEG : 
		 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD && 
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

			return - fg->eval(as,at);
	case MULT : 
		 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

			return fg->eval(as,at) * fd->eval(as,at);
	case DIV : 
		 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

			return fg->eval(as,at) / fd->eval(as,at);
	case MOD : 
		 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

			return fg->eval(as,at) % fd->eval(as,at);
	case EQUAL : 
		 if((fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT))
  			return (int)(fg->eval(as,at) == fd->eval(as,at));
     else
        return (int)(eval(at,fd) == eval(at,fg));
     
	case LESSER :
		 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);
			return (int)(fg->eval(as,at) < fd->eval(as,at));
        case GREATER :
		 assert(fg->t != FIFO && fd->t !=FIFO && fd->t != WORD &&
		 fg->t != WORD && fd->t != CONCAT && fg->t != CONCAT);

			return (int)(fg->eval(as,at) > fd->eval(as,at));
	case NOT : 
		return (int)(!fg->eval(as,at));
	case AND : 
		if (fg->eval(as,at))
			return fd->eval(as,at);
		else
			return 0;
	case OR : 
		if (fg->eval(as,at))
			return 1;
		else
			return fd->eval(as,at);
       case HEAD :
		assert(fg->t != SIZE && fd->t !=SIZE && fd->t != VAR &&
                 fg->t != VAR &&  fg->t != CONST && fd->t !=CONST);

           	     return  (int) eval(at,fd).head(eval(at,fg));

       case QUEUE:
		assert(fg->t != SIZE && fd->t !=SIZE && fd->t != VAR && 
                fg->t != VAR &&  fg->t != CONST && fd->t !=CONST);

	                return (int) eval(at,fd).queue(eval(at,fg));

       case BELONGS:
		assert(fg->t != SIZE && fd->t !=SIZE && fd->t != VAR && 
		fg->t != VAR &&  fg->t != CONST && fd->t !=CONST);

	                return eval(at,fd).belongs(eval(at,fg));

       case SIZE:
                return get_fifo(at,v).size();
       default:
              return false;
	}
	return 0;
}
Word Vertex::eval(const Ass_fifo& as, Tree tree) const {
	switch (tree->t) {
           case FIFO:
                 return get_fifo(as,tree->v);
           case WORD:
                 return tree->word ;
           case CONCAT :
                 return eval(as,tree->fg)&eval(as,tree->fd);
           default:
               return false;
        }
    return 0;
}

Tree Vertex::clone() const {
	if (t==CONST || t==VAR || t==FIFO || t == SIZE )
		return new Vertex(t, v);
        if(t == WORD)
    return new Vertex(t, word);
	return new Vertex(t, v, fg->clone(), fd==NULL?NULL:fd->clone());
}

Vertex::~Vertex() {
	if (t != CONST && t != VAR && t != FIFO && t !=WORD && t !=SIZE) {
		delete fg;
		if (fd) delete fd;
	}
}

bool Vertex::operator==(const Vertex& a) const {
	if (this==&a)
		return true;
	if (t!=a.t)
		return false;
	if (t == CONST || t == VAR || t== SIZE || t ==FIFO)
		return (v == a.v);
  if(t == WORD)
    return (word == a.word);
	return (*fg == *(a.fg) && (fd==NULL&&a.fd==NULL)?true:*fd == *(a.fd));
}

int Vertex::hash() const { 
	if (t == CONST || t == VAR || t == SIZE || t == FIFO)
		return t*9119^v; //91019
  if(t == WORD )  
    return (t+1)*1991^word.as_WordHash();//19099
	return t * fg->hash() * (fd==NULL?1:fd->hash());
}

/*****************************************************************************************************/
//______________________________Class Expression_______________________________ 

Expression::Expression(int i, Type t) {
	assert(t==CONST || t == VAR || t == FIFO || t == SIZE);
	if (t==VAR)
		insert(is, i);
        if(t == SIZE || t == FIFO)
                insert(fifo, i);       
	root = new Vertex(t, i);
}
Expression::Expression(Word w, Type) {
      root = new Vertex(WORD,w);
}
Expression::Expression(const Expression& e) {
	is = e.is;
        fifo = e.fifo;
	assert(e.root);
	root = e.root->clone();
}

Expression& Expression::operator=(const Expression& e) {
	if (this != &e) {
		delete root;
		is = e.is;
                fifo =e.fifo;
		assert(e.root);
		root = e.root->clone();
	}
	return *this;
}

Expression::~Expression() {
	delete root;
}

bool Expression::isParam(int var) const {
	return belongs(is, var);
}
bool Expression::isParam_fifo(int var) const {
	return belongs(fifo, var);
}
unsigned int Expression::size() const {
	return is.size()+fifo.size();
}

int Expression::eval(const Assignment &as,const Ass_fifo &at) const {
	assert(root);
	return root->eval(as,at);
}

size_t Expression::hash() const {
	int res = 1;
	IntSet::const_iterator ii,jj;
	for ( ii = is.begin(),jj = fifo.begin();ii!=is.end() && jj!=fifo.end();ii++,jj++)
		res *= *ii*(*jj); 
  assert(root);
	return res ^root->hash();
}

bool Expression::isEqual(const Expression &e) const {
	return is == e.is && fifo == e.fifo  && *root == *(e.root) ;
}

Expression::Expression() {}

Expression::Expression (Tree root) {
  this->root = root->clone();
  pair<IntSet,IntSet> targets;
  root->findTargets (targets);
  is = targets.first;
  fifo = targets.second;
}

Expression Expression::gen_un_op(Type ty) const {
	Expression r;
	r.is = is;
  r.fifo = fifo;
	r.root = new Vertex(ty, 0, root->clone(), NULL); 
	return r;
}

Expression Expression::gen_bin_op(const Expression& od, Type ty) const {
	Expression r;
	insert_iterator<set<int> > i(r.is, r.is.end());
	set_union(is.begin(),is.end(),od.is.begin(),od.is.end(),i);

        insert_iterator<set<int> > j(r.fifo, r.fifo.end());
	set_union(fifo.begin(),fifo.end(),od.fifo.begin(),od.fifo.end(),j);

	r.root = new Vertex(ty, 0, root->clone(), od.root->clone()); 
	return r;
}

//------------------------Operator -----------------------------	
Expression operator+(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, PLUS);
}

Expression operator-(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, MINUS);
}

Expression operator-(const Expression& o) {
	return o.gen_un_op(NEG);
}

Expression operator*(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, MULT);
}

Expression operator/(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, DIV);
}

Expression operator%(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, MOD);
}

Expression operator==(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, EQUAL);
}

Expression operator!(const Expression& o) {
	return o.gen_un_op(NOT);
}

Expression operator<(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, LESSER);
}
Expression operator>(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, GREATER);
}

Expression operator&&(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, AND);
}

Expression operator||(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, OR);
}

//-------------------Operator Queue------------------------

Expression head(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, HEAD);
}
Expression queue(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, QUEUE);
}
Expression belongs(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, BELONGS);
}
Expression operator&(const Expression& og, const Expression& od) {
	return og.gen_bin_op(od, CONCAT);
}
