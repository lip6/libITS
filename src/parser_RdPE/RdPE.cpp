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
 


#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
//#include <ext/hash_map>
#include <algorithm>
#include "RdPE.h"

/***********************************************************/
/*                      class Node                         */
/***********************************************************/

void  Node::addPre(int node,int valuation){
  pair<int,int> x(node,valuation);
  pre.push_back(x);
}

void Node::addPost(int node,int valuation){
  pair<int,int> x(node,valuation);
  post.push_back(x);
}

void  Node::addInhibitor(int node,int valuation){
  pair<int,int> x(node,valuation);
  inhibitor.push_back(x);
}

void  Node::addPreAuto(int node,int valuation){
  pair<int,int> x(node,valuation);
  preAuto.push_back(x);
}

void Node::addPostAuto(int node,int valuation){
  pair<int,int> x(node,valuation);
  postAuto.push_back(x);
}

void Node::addTest(int node,int valuation){
  pair<int,int> x(node,valuation);
  test.push_back(x);
}

void Node::addReset(int node){
  reset.push_back(node);
}

/***********************************************************/
/*                      class RdPE                         */
/***********************************************************/
RdPE::RdPE(const char *f){
  if(create(f)){
    for (vector<class Place>::iterator p=places.begin();p!=places.end();p++){	
      sort(p->pre.begin(),p->pre.end());
      sort(p->post.begin(),p->post.end());
    }
    for (vector<class Transition>::iterator p=transitions.begin();p!=transitions.end();p++){	
      sort(p->pre.begin(),p->pre.end());
      sort(p->post.begin(),p->post.end());
    }
  }
  else{
    places.clear();
    transitions.clear();
    placeName.clear();
    transitionName.clear();
  }
}

bool RdPE::addPlace(const string &place,int marking,int capacity,int m){
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end()){
    int pindex = modindex.size();
    if (modules.find(m) == modules.end()) {
      modules[m] = vector<int> (1,pindex);
      modindex.push_back(make_pair(m,0));
    } else {
      modindex.push_back(make_pair(m,modules[m].size()));
      modules[m].push_back(places.size());
    }
    placeName[place]=pindex;
    Place p(place,marking, capacity);
    places.push_back(p);
    return true;
  }
  else
    return false;
}

bool RdPE::addQueue(const string &place,int capacity,int m){
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end()){
    int pindex = places.size();
    if (modules.find(m) == modules.end()) {
      modules[m] = vector<int> (1,pindex);
      modindex.push_back(make_pair(m,0));
    } else {
      modindex.push_back(make_pair(m,modules[m].size()));
      modules[m].push_back(places.size());
    }
    placeName[place]=pindex; 
    Place p(place,-1, capacity);
    places.push_back(p);
    return true;
  }
  else
    return false;
}

bool RdPE::addLossQueue(const string &place,int capacity,int m){
  map<string,int>::const_iterator pi=placeName.find(place);

  if(pi==placeName.end()){
    int pindex = places.size();
    if (modules.find(m) == modules.end()) {
      modules[m] = vector<int> (1,pindex);
      modindex.push_back(make_pair(m,0));
    } else {
      modindex.push_back(make_pair(m,modules[m].size()));
      modules[m].push_back(places.size());
    }
    placeName[place]=pindex;
    Place p(place,-2, capacity);
    places.push_back(p);
    return true;
  }
  else
    return false;
}

bool RdPE::addTrans(const string &trans){
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end()){
    transitionName[trans]=transitions.size();
    Transition t(trans);
    transitions.push_back(t);
    return true;
  }
  else
    return false;
}

bool RdPE::addPre(const string &place,const string &trans,int valuation){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addPre(p,valuation);
  places[p].addPost(t,valuation);
  return true;
}

bool RdPE::addTest(const string &place,const string &trans,int valuation){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addTest(p,valuation);
  places[p].addTest(t,valuation);
  return true;
}


bool RdPE::addPost(const string &place,const string &trans,int valuation){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addPost(p,valuation);
  places[p].addPre(t,valuation);
  return true;
}

bool RdPE::addPreQueue(const string &place,const string &trans,int valuation){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || !places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addPre(p,valuation);
  places[p].addPost(t,valuation);
  return true;
}

bool RdPE::addPostQueue(const string &place,const string &trans,int valuation){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || !places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addPost(p,valuation);
  places[p].addPre(t,valuation);
  return true;
}

bool RdPE::addInhibitor(const string &place,const string &trans,int valuation){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addInhibitor(p,valuation);
  places[p].addInhibitor(t,valuation);
  return true;
}

bool RdPE::addPreAuto(const string &place,const string &trans,const string &valuation){
  int p,t,v;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  map<string,int>::const_iterator pv=placeName.find(valuation);
  if(pv==placeName.end() || places[pv->second].isQueue())
    return false;
  else
    v=pv->second;
  transitions[t].addPreAuto(p,v);
  places[p].addPostAuto(t,v);
  return true;
}

bool RdPE::addPostAuto(const string &place,const string &trans,const string &valuation){
  int p,t,v;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end() || places[pi->second].isQueue())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  map<string,int>::const_iterator pv=placeName.find(valuation);
  if(pv==placeName.end() || places[pi->second].isQueue())
    return false;
  else
    v=pv->second;
  transitions[t].addPostAuto(p,v);
  places[p].addPreAuto(t,v);
  return true;
}

bool RdPE::addReset(const string &place,const string &trans){
  int p,t;
  map<string,int>::const_iterator pi=placeName.find(place);
  if(pi==placeName.end())
    return false;
  else
    p=pi->second;
  map<string,int>::const_iterator ti=transitionName.find(trans);
  if(ti==transitionName.end())
    return false;
  else
    t=ti->second;
  transitions[t].addReset(p);
  places[p].addReset(t);
  return true;
}

/* Visualisation */
ostream& operator<<(ostream &os,const RdPE &R){

/* affichage nombre de places et de transitions */
  os<<"***************************"<<endl;
  os<<"Nombre de modules     :"<<R.modules.size()<<endl;
  os<<"Nombre de places     :"<<R.nbPlace()<<endl;
  os<<"Nombre de transitions:"<<R.nbTransition()<<endl;

/* affichage de la liste des places */
  os<<"********** places **********"<<endl;
  int i=0;
   for (vector<class Place>::const_iterator p=R.places.begin();p!=R.places.end();p++,i++){
    if (p->isQueue()) {
       	os<<"queue " << setw(4)<<i<<":"<<p->name<<", cp("<<p->capacity<<")";
		if (p->isLossQueue())
			cout << " loss"; 
		cout << " (module " << R.getmodule(i) << " / " << R.getmodindex(i) <<") " << endl;
	}
	else
	  os<<"place " << setw(4)<<i<<":"<<p->name<<":"<<p->marking<<" <..>, cp(" << p->capacity<<")" <<" (module " << R.getmodule(i) << " / " << R.getmodindex(i) << ") " <<endl;
  }
  os<<"********** modules **********"<<endl;
  i=0;
  for (map<int, vector<int> >::const_iterator  mod = R.modules.begin() ; mod != R.modules.end() ; mod++,i++) {
    os << "module " << setw(4)<<i<<":";
    for (vector<int>::const_iterator pl = mod->second.begin() ; pl != mod->second.end() ; pl++) {
      os << *pl << " ";
    }
    os << endl;
  }

  os<<"********** transitions **********"<<endl;
  i=0;
  for (vector<class Transition>::const_iterator p=R.transitions.begin();
       p!=R.transitions.end();p++,i++){	
    os<<setw(4)<<i<<":"<<p->name<<endl;
    os<<"        IN  { ";
    for (vector< pair<int,int> >::const_iterator a=p->pre.begin();a!=p->pre.end();a++)
	  if (R.places[a->first].isQueue())
        os<<R.places[a->first].name<<":<"<<a->second<<">;";
	  else
        os<<R.places[a->first].name<<":"<<a->second<<";";
    for (vector< pair<int,int> >::const_iterator a=p->inhibitor.begin();a!=p->inhibitor.end();a++)
      os<<R.places[a->first].name<<"<"<<a->second<<";";
    for (vector< pair<int,int> >::const_iterator a=p->preAuto.begin();a!=p->preAuto.end();a++)
      os<<R.places[a->first].name<<":"<<R.places[a->second].name<<";";
    for (vector<int>::const_iterator a=p->reset.begin();a!=p->reset.end();a++)
      os<<R.places[*a].name<<": reset;";
    os<<" }"<<endl;
    
    os<<"        OUT { ";
    for (vector< pair<int,int> >::const_iterator a=p->post.begin();a!=p->post.end();a++)
	  if (R.places[a->first].isQueue())
        os<<R.places[a->first].name<<":<"<<a->second<<">;";
	  else
        os<<R.places[a->first].name<<":"<<a->second<<";";
    for (vector< pair<int,int> >::const_iterator a=p->postAuto.begin();a!=p->postAuto.end();a++)
      os<<R.places[a->first].name<<":"<<R.places[a->second].name<<";";
    os<<" }"<<endl;
  }
  return(os);
}

bool RdPE::setEft(int tr,int eft) { 
  if (tr >= nbTransition() ) {
    std::cerr << "Unknown transition id " << tr << " when setting earliest firing time\n";
    return false;
  }
  Transition & t = transitions[tr];
  t.clock.eft = eft;
  return true;
}
bool RdPE::setLft(int tr,int lft) { 
  if (tr >= nbTransition() ) {
    std::cerr << "Unknown transition id " << tr << " when setting latest firing time\n";
    return false;
  }
  Transition & t = transitions[tr];
  t.clock.lft = lft;
  return true;
}
bool RdPE::setVisibility(int tr,bool isPublic) { 
  if (tr >= nbTransition() ) {
    std::cerr << "Unknown transition id " << tr << " when setting earliest firing time\n";
    return false;
  }
  Transition & t = transitions[tr];
  t.isPublic = isPublic;
  return true;
}






