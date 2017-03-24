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
*  subject      : class  syntaxe_CAMI
*  version      : 0.1
****************************************************************************************/
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

#include "RdPE.h"
#include "Cami.hpp"

/**************************************************************************************/
/*                               class  syntaxe_CAMI                                  */
/**************************************************************************************/
namespace cami {

CAMI::CAMI(const char *f){
  if(! create(f)){
    valide=0;
    places.clear();
    transitions.clear();
  }
  else {
    for (vector<class _Place>::iterator p=places.begin();p!=places.end();p++){
      sort(p->pre.begin(),p->pre.end());
      sort(p->post.begin(),p->post.end());
    }
    for (vector<class _Transition>::iterator p=transitions.begin();p!=transitions.end();p++){
      sort(p->pre.begin(),p->pre.end());
      sort(p->post.begin(),p->post.end());
    }
    valide=1;
  }
}

bool CAMI::addPlace(int place, string name,int marking,int capacity){
  vector<class _Place>::iterator pi;
  for(pi=places.begin(); pi !=places.end() && (*pi).Num !=place ;pi++) { }
  if(pi==places.end()){
    _Place p(place,name,marking, capacity);
    places.push_back(p);
    return true;
  }
  else {
  	if( (*pi).marking ==0 || (*pi).capacity ==0 ) {
		  if( (*pi).marking ==0 )
			  (*pi).markingPlace(marking);
		  if( (*pi).capacity ==0 )
			  (*pi).capacityPlace(capacity);
		return true;
	}
  }
    return false;
}

bool CAMI::addQueue(int place, string name,int capacity){
  vector<class _Place>::iterator pi;
  for( pi=places.begin(); pi !=places.end() && (*pi).Num !=place ;pi++) { }
  if(pi==places.end()){
    _Place p(place,name,-1, capacity);
    places.push_back(p);
    return true;
  }
  else {
  	if((*pi).capacity ==0 ) {
		(*pi).capacityPlace(capacity);
		return true;
	}
  }
    return false;
}

bool CAMI::addLossQueue(int place,string name,int capacity){
  vector<class _Place>::iterator pi;
  for(pi=places.begin(); pi !=places.end() && (*pi).Num !=place ;pi++) { }
  if(pi==places.end()){
    _Place p(place,name,-2, capacity);
    places.push_back(p);
    return true;
  }
  else {
  	if((*pi).capacity ==0  ) {
		(*pi).capacityPlace(capacity);
		return true;
	}
   }
    return false;
}

bool CAMI::addTrans(int trans,string name){
  vector<class _Transition>::iterator  pi;
  for(pi=transitions.begin();pi !=transitions.end() && (*pi).Num !=trans; pi++) { }
  if(pi==transitions.end()){
    _Transition t(trans,name);
    transitions.push_back(t);
    return true;
  }
  return false;
}

bool CAMI::addPre(int arc,int place,int trans,int valuation){
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
  if(pi==places.end() )
	return false;

  vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
  if(ti==transitions.end())
	return false;
    pair<int,int> x(place,trans);
    pair<typeArc,pair<int,int> > y(preArc,x);
    Arc[arc]=y;

  (*ti).addPre(arc,valuation);
  (*pi).addPost(arc,valuation);

  return true;

}

bool CAMI::addPost(int arc,int place,int trans,int valuation){
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
  if(pi==places.end() )
    return false;

    vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
  if(ti==transitions.end())
    return false;

  pair<int,int> x(place,trans);
  pair<typeArc,pair<int,int> > y(postArc,x);
  Arc[arc]=y;

  (*ti).addPost(arc, valuation);
  (*pi).addPre(arc, valuation);

  return true;
}

bool CAMI::addPreAuto(int arc,int place,int trans,int placeAut){
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
  if(pi==places.end() || !(*pi).isQueue() )
    return false;
  vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
  if(ti==transitions.end())
    return false;

  pair<int,int> x(place,trans);
  pair<typeArc,pair<int,int> > y(preAut,x);
  Arc[arc]=y;

  (*ti).addPreAuto(arc,placeAut);
  (*pi).addPostAuto(arc,placeAut);
  return true;
}

bool CAMI::addPostAuto(int arc,int place,int trans,int placeAut){
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
  if(pi==places.end() || (*pi).isQueue() )
    return false;

  vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
  if(ti==transitions.end())
    return false;

  pair<int,int> x(place,trans);
  pair<typeArc,pair<int,int> > y(postAut,x);
  Arc[arc]=y;

  (*ti).addPostAuto(arc,placeAut);
  (*pi).addPreAuto(arc,placeAut);
  return true;
}


bool CAMI::addInhibitor(int arc,int place,int trans,int valuation){
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
  if(pi==places.end())
    return false;

  vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
  if(ti==transitions.end())
    return false;

  pair<int,int> x(place,trans);
  pair<typeArc,pair<int,int> > y(inhArc,x);
  Arc[arc]=y;

  (*ti).addInhibitor(arc,valuation);
  (*pi).addInhibitor(arc,valuation);
  return true;
}


bool CAMI::addReset(int arc,int place,int trans){
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
  if(pi==places.end() )
    return false;

  vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
  if(ti==transitions.end())
    return false;

  pair<int,int> x(place,trans);
  pair<typeArc,pair<int,int> > y(resetArc,x);
  Arc[arc]=y;

  (*ti).addReset(arc);
  (*pi).addReset(arc);
  return true;
}
/*----------------------------------------------------------------*/
bool CAMI::nameNode(int node, string name) {
  vector<class _Place>::iterator pi;
  for(pi=places.begin();pi !=places.end() && (*pi).Num !=node; pi++) { }
  if( pi !=places.end()) {
       	(*pi).namePlace(name);
       	return 1;
  }
  vector<class _Transition>::iterator ti;
  for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=node; ti++) { }
  if( ti !=transitions.end()) {
       	(*ti).nameTransition(name);
       	return 1;
  }
 return 0;
}

/*----------------------------------------------------------------------*/

_Place& CAMI::get_place(int place){
	vector<class _Place>::iterator pi;
	for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
	return (*pi);
}
bool  CAMI::belongs_PLACE(int place) {
	vector<class _Place>::iterator pi;
	for(pi=places.begin();pi !=places.end() && (*pi).Num !=place; pi++) { }
	if( pi !=places.end())
		return 1;
	return 0;
}
_Transition& CAMI::get_transition(int trans) {
	vector<class _Transition>::iterator ti;
	for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
	return (*ti);
}
bool CAMI::belongs_TRANS(int trans) {
	vector<class _Transition>::iterator ti;
	for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=trans; ti++) { }
	if( ti !=transitions.end())
		return 1;
	return 0;
}
bool CAMI::belongs_Arc(int arc){
  map_Arc::iterator ai=Arc.find(arc);
  if(ai!=Arc.end())
    return 1;
  return 0;
}
/*................................function of setValution the arc...................................*/

bool CAMI::setValuation(int arc, int valuation) {

	map_Arc::iterator ai=Arc.find(arc);
	if(ai != Arc.end()) {
		vector<class _Place>::iterator pi;
		vector<class _Transition>::iterator ti;
		for(pi=places.begin();pi !=places.end() && (*pi).Num !=(ai->second).second.first; pi++) { }
		for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=(ai->second).second.second; ti++) { }
    if( ti ==transitions.end() || pi ==places.end())
      return 0;
			if((ai->second).first == postArc) {
				 if( !(eraseNode( (*ti).post,arc) && eraseNode((*pi).pre,arc) ) )
					    return 0;
				(*ti).addPost( arc, valuation);
  		  (*pi).addPre( arc, valuation );
			}
			else {
				if((ai->second).first == preArc ) {
					if( !(eraseNode((*ti).pre,arc) &&  eraseNode((*pi).post,arc) ) )
					      return 0;
					(*ti).addPre( arc,valuation);
  			  (*pi).addPost(arc,valuation);

				}
				else
						if((ai->second).first == inhArc) {
							if( !(eraseNode((*ti).inhibitor,arc) && eraseNode( (*pi).inhibitor,arc) ) )
								    return 0;
							(*ti).addInhibitor( arc,  valuation );
  					  (*pi).addInhibitor( arc,  valuation );
						}
			}
		return 1;
	}
	return 0;
}
bool CAMI::setValuation(int arc,string& valuation) {
    map_Arc::iterator ai=Arc.find(arc);
    vector<class _Place>::iterator vi;
    for(vi=places.begin();vi !=places.end() && (*vi).name != valuation; vi++) { }
	  if(ai != Arc.end() && vi !=places.end()) {
		    vector<class _Place>::iterator pi;
		    vector<class _Transition>::iterator ti;
		    for(pi=places.begin();pi !=places.end() && (*pi).Num !=(ai->second).second.first; pi++) { }
		    for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=(ai->second).second.second; ti++) { }
        if( ti ==transitions.end() || pi ==places.end())
          return 0;
        if((ai->second).first == postArc) {
				  if( !(eraseNode((*ti).post,arc) && eraseNode((*pi).pre,arc) && eraseNode(Arc,arc) ) )
					    return 0;
  			  (*ti).addPostAuto(arc,(*vi).Num);
    	    (*pi).addPreAuto(arc, (*vi).Num);
          pair<int,int> x((*pi).Num,(*ti).Num);
          pair<typeArc,pair<int,int> > y(postAut,x);
          Arc[arc]=y;
			  }
			  else {
				  if((ai->second).first == preArc ) {
					    if( !(eraseNode((*ti).pre,arc) &&  eraseNode((*pi).post,arc) && eraseNode(Arc,arc) ) )
					        return 0;
					    (*ti).addPreAuto( arc,(*vi).Num);
  			      (*pi).addPostAuto(arc,(*vi).Num);
              pair<int,int> x((*pi).Num,(*ti).Num);
              pair<typeArc,pair<int,int> > y(preAut,x);
              Arc[arc]=y;
				}
      }
      return 1;
    }
  return 0;
}
bool CAMI::setValuationRESET(int arc){
  map_Arc::iterator ai=Arc.find(arc);
	if(ai != Arc.end()) {
        vector<class _Place>::iterator pi;
		    vector<class _Transition>::iterator ti;
		    for(pi=places.begin();pi !=places.end() && (*pi).Num !=(ai->second).second.first; pi++) { }
		    for(ti=transitions.begin(); ti !=transitions.end() && (*ti).Num !=(ai->second).second.second; ti++) { }
        if(!(pi !=places.end() && ti !=transitions.end() && (*ti).post.size()==0 && (*ti).postAuto.size() ==0) ){
            return 0;
        }
        if((ai->second).first == preArc) {
					  if( !(eraseNode((*ti).pre,arc) &&  eraseNode((*pi).post,arc) && eraseNode(Arc,arc) ) ) {
					    return 0;
            }
            (*ti).addReset(arc);
            (*pi).addReset(arc);
            pair<int,int> x((*pi).Num,(*ti).Num);
            pair<typeArc,pair<int,int> > y(resetArc,x);
            Arc[arc]=y;
        }
        return 1;
  }
	return 0;
}
bool eraseNode(vector<pair< int,int> > &Container,int node){
	for(vector<pair< int,int> >::iterator it=Container.begin(); it !=Container.end(); it++) {
	       if(it->first == node) {
	       	  Container.erase(it);
		      return 1;
	       }
	}
  return 0;
}
bool eraseNode(map_Arc &Container,int node){
	for(map_Arc::iterator it=Container.begin(); it !=Container.end(); it++) {
	       if(it->first == node) {
	       	  Container.erase(it);
		      return 1;
	       }
	}
return 0;
}
/*.....................................function of conversion............................................*/
RdPE CAMI_RdPE(CAMI& C) {
  RdPE R;
  for(vector<class _Place>::iterator pi=C.places.begin() ; pi!=C.places.end() ; pi++){
    R.addPlace((*pi).name,(*pi).marking,(*pi).capacity);
  }
  for(vector<class _Transition>::iterator ti=C.transitions.begin();ti !=C.transitions.end(); ti++){
    R.addTrans((*ti).name);
  }
  for(map_Arc::const_iterator ai=C.Arc.begin();ai !=C.Arc.end();ai++) {
    if((ai->second).first==preArc) {
	     vector<pair<int,int> >::iterator vi;
	     for(vi= C.get_place((ai->second).second.first).post.begin();vi!=C.get_place((ai->second).second.first).post.end()&& vi->first!=ai->first;vi++){}
	      if(C.get_place((ai->second).second.first).isQueue())
	         R.addPreQueue(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,vi->second);
        else
	         R.addPre(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,vi->second);
      }
    else {
        if((ai->second).first==postArc) {
	         vector<pair<int,int> >::iterator vi;
	         for(vi=C.get_place((ai->second).second.first).pre.begin();vi!=C.get_place((ai->second).second.first).pre.end()&& vi->first!=ai->first;vi++){}
	         if(C.get_place((ai->second).second.first).isQueue())
	             R.addPostQueue(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,vi->second);
           else
               R.addPost(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,vi->second);
        }
         else {   
             if((ai->second).first==inhArc) {
	            vector<pair<int,int> >::iterator vi;
	            for(vi=C.get_place((ai->second).second.first).inhibitor.begin();vi!=C.get_place((ai->second).second.first).inhibitor.end()&& vi->first!=ai->first;vi++){}
	            R.addInhibitor(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,vi->second);
              }
              else {   
                   if((ai->second).first==preAut) {
		                    vector<pair<int,int> >::iterator vi;
		                    for(vi=C.get_place((ai->second).second.first).postAuto.begin();vi!=C.get_place((ai->second).second.first).postAuto.end()&& vi->first!=ai->first;vi++){}
		                    R.addPreAuto(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,C.get_place(vi->second).name);
                    }
                    else {
                        if((ai->second).first==postAut) {
		                      vector<pair<int,int> >::iterator vi;
		                      for(vi=C.get_place((ai->second).second.first).preAuto.begin();vi!=C.get_place((ai->second).second.first).preAuto.end()&& vi->first!=ai->first;vi++){}
		                      R.addPostAuto(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name,C.get_place(vi->second).name);
                        }
                        if((ai->second).first==resetArc) {
                          R.addReset(C.get_place((ai->second).second.first).name,C.get_transition((ai->second).second.second).name);
                        } 
                    }
              }
          }
      }
  }
  return (R);
}


} // namespace



