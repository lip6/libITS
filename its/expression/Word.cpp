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
*  subject      : class  Word
*  version      : 0.1
****************************************************************************************/

#include <cassert>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;
#include "Word.hpp"

//************************************Class Word*************************************************
Word::Word(int val) {
        w.push_back(val);
}
Word::Word() { 
}
Word Word::operator&(const Word& word) const {
   Word  job;
    job.w = w;
    for(vector<int>::const_iterator it=(word.w).begin(); it !=(word.w).end(); it++)     
                (job.w).push_back(*it);        
      return(job);
}

bool Word::operator==(const Word& word) const {
	return w == word.w;
}

ostream& operator<< (ostream& o, const Word& w) {
	for(vector<int>::const_iterator it=w.w.begin(); it !=w.w.end(); it++) 
		o << *it << '.';
	return o;
}
//-----------------------------------------------------------
size_t Word::as_WordHash( )const {
	size_t r=1;
	for(vector<int>::const_iterator it=w.begin(); it != w.end(); it++) {
		r+=(*it*(int)pow((double)10,(int)r));
	}
	return r*19919;	
}	
//-------------------------head fuction------------------------------
 bool  Word::head(const Word  &val) const{  
          if(val.w.size()==0)
                return true; 
	  else {
		if(w.size() < val.w.size()|| w.size()==0)
			return false;
	}
	
	vector<int>::const_iterator it,at;
	for( it= w.begin(), at=(val.w).begin();it!= w.end()&&at!= (val.w).end() ; it ++,at++) {
                   if(*it!=*at)
			return false;     
         }
   return true;
}
//-------------------------queue function----------------------------

 bool  Word::queue(const  Word& val) const{

        if(val.w.size()==0)
               return true;
	 else {
		if(w.size()<val.w.size()|| w.size()==0)
			return false;
	 }
	vector<int>::const_reverse_iterator it,at;
	for( it=(val.w).rbegin(), at=w.rbegin(); it!=(val.w).rend()&&at!= w.rend() ;it++,at++) {
                   if(*it!=*at)
                               return false; 
           }
   return true;
}
//------------------------belongs function----------------------------
 bool  Word::belongs(const Word  &val) const{
 
            if( val.w.size()==0)
                     return true;
            else {
                    if(val.w.size() > w.size()|| w.size()==0)
                          return false;
             }
             if( search(w.begin(),w.end(),val.w.begin(),val.w.end() ) != w.end() )
		return true;
   return false;
}

//---------------------------size of fifo-----------------------------
 int  Word::size()const{
      return(w.size());
}



