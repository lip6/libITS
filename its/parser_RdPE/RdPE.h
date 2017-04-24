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
#ifndef RdPE_H
#define RdPE_H
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "its/parser_RdPE/RdPMonteur.h"
using namespace std;

class Node{
public:
  Node(){};
  virtual ~Node(){};
  vector< pair<int,int> > pre, post, inhibitor, preAuto, postAuto, test;
  vector<int> reset;
  void addPre(int,int);
  void addPost(int,int);
  void addInhibitor(int,int);
  void addPreAuto(int,int);
  void addPostAuto(int,int);
  void addReset(int);
  void addTest(int,int);
};

class Place:public Node{
public:
  string name;
  int marking, capacity;
  Place(const string &p,int m=0, int c=0):name(p),marking(m),capacity(c) {};
  ~Place(){};
  bool isLossQueue() const {return marking==-2;}
  bool isQueue() const {return marking<=-1;}
  bool hasCapacity() const {return capacity!=0;}
};

class Clock {
 public:
  int eft;
  // -1 means infinity
  int lft;

  Clock(int eft=0, int lft=-1): eft(eft), lft(lft) {};
    bool isConstraint() const {
      return eft != 0 || lft != -1;
    }
};

class Transition:public Node{          
public:
  string name;
  // handle visibility
  bool isPublic;
  Clock clock;
 Transition(const string &t):name(t),isPublic(false){};
  ~Transition(){};
};


class RdPE: public RdPMonteur {
public:
  /* Attributs */ 
  vector<class Place> places;
  vector<class Transition> transitions;
  map<string,int> placeName;
  map<string,int> transitionName;
  // maps a place index to a <module,index in module> pair
  vector<pair<int,int> > modindex;
  // map a module index to the indexes of the places composing it 
  map<int, vector<int> > modules;

  int getPlaceIndex (string pname) const {
    return placeName.find(pname)->second;
  }

  // const accessors
  int getmodule (int pl) const {
    return modindex[pl].first;
  }
  int getmodindex (int pl) const {
    return modindex[pl].second;
  }
  
  /* Constructors */  
  RdPE(){};
  virtual ~RdPE(){};
  RdPE(const char *file);
  /* Monteur */
  bool addPlace(const string &place,int marking=0, int capacity=0,int mod=0);
  bool addQueue(const string &place,int capacity=0,int mod=0);
  bool addLossQueue(const string &place,int capacity=0,int mod=0);
  bool addTrans(const string &transition);
  bool setEft(int tr,int eft);
  bool setLft(int tr,int lft);
  bool setVisibility(int tr,bool isPublic);
  bool addPre(const string &place,const string &transition,int valuation=1);
  bool addPost(const string &place,const string &transition,int valuation=1);  
  bool addPreQueue(const string &place,const string &transition,int valuation=1);
  bool addPostQueue(const string &place,const string &transition,int valuation=1);  
  bool addInhibitor(const string &place,const string &transition,int valuation=1);
  bool addTest(const string &place,const string &transition,int valuation=1);
  bool addPreAuto(const string &place,const string &transition,const string &valuation);
  bool addPostAuto(const string &place,const string &transition,const string &valuation);  
  bool addReset(const string &place,const string &transition);
  
  /* Visualisation */
  size_t nbPlace() const {return places.size();};
  size_t nbTransition() const {return transitions.size();};
  bool isTimePetriNet () const ;
};

ostream& operator<<(ostream &,const RdPE &);
#endif
