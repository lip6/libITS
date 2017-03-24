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
// version      : 0.1

#ifndef __CAMI_HPP__
#define __CAMI_HPP__

#include <string>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "its/parser_RdPE/RdPE.h"

namespace cami {

struct LossCty{
  int capacity;
  bool loss;
};

enum typeArc{postArc, preArc, inhArc, resetArc, preAut, postAut};
typedef map<int,pair<typeArc,pair<int,int> > > map_Arc;

class CamiMonteur {
public:
 bool create(const char *f);
};
//----------------------------------------class _Place-----------------------------------------

class _Place:public Place{
public:
   int  Num;
  _Place(int  number,string &nom, int mkg=0 , int cty=0 ):Place(nom,mkg,cty){Num=number;};
  ~_Place(){};
  void namePlace(string  n) {name=n;}
  void markingPlace(int j){marking=j;}
  void capacityPlace(int j) {capacity=j;}
};
//----------------------------------------class _transition------------------------------------
class _Transition:public Transition{
public:
   int  Num;
  _Transition(int number,string &nom):Transition(nom){Num=number;};
  ~_Transition(){};
  void nameTransition(string nom){name=nom;}
};


class CAMI: public CamiMonteur{
public:
  /* Attributs */
  bool valide;  //pour le parser Expression
  vector<class _Place> places;
  vector<class _Transition> transitions;
  map_Arc  Arc;  //pour les fonctions setValuation
  /* Constructors */
  CAMI(){};
  ~CAMI(){};
  CAMI(const char *file);
  /* Monteur */
  bool addPlace(int place, string name="",int marking=0, int capacity=0);
  bool addQueue(int place, string name="",int capacity=0);
  bool addLossQueue(int place, string name="",int capacity=0);
  bool addTrans(int transition, string name="");
  bool addPre(int arc,int place,int transition,int valuation=1);
  bool addPost(int arc,int place,int transition,int valuation=1);
  bool addInhibitor(int arc,int place,int transition,int valuation=1);
  bool addPreAuto(int arc,int place,int transition,int placeAut);
  bool addPostAuto(int arc,int place,int transition,int placeAut);
  bool addReset(int arc,int place,int transition);
  bool nameNode(int , string);
  bool belongs_PLACE(int);
  bool belongs_TRANS(int);
  bool belongs_Arc(int);
  bool setValuation(int,int);
  bool setValuation(int,string&);
  bool setValuationRESET(int);
  bool valide_CAMI()const {return valide;}
  _Place& get_place(int);
  _Transition& get_transition(int);
  /************************* Visualisation************************* */
  int nbPlace() const {return places.size();};
  int nbTransition() const {return transitions.size();};
  int nbArc()const{return Arc.size();};
};

bool eraseNode(vector<pair< int,int> > &,int);
bool eraseNode(map_Arc &,int);
/***********************conversion de CAMI vers RdPE****************/
RdPE CAMI_RdPE(CAMI&);


}
  
#endif



