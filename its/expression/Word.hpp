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
// version 0.1

#ifndef _WORD_
#define _WORD_

#include <iostream>
#include <cassert>
#include <map>
#include <vector> 

/**************************************Class Word***********************************************/	
class Word {
public:
       Word(int val);
       Word();
        bool belongs(const Word  &val)const;
        bool queue(const  Word  &val)const;
        bool head(const Word  &val)const;
        Word operator&(const Word& w)const;
        bool operator==(const Word& w) const; 
	size_t as_WordHash()const;
        int  size()const;
	friend ostream& operator<< (ostream& o, const Word& w);
private:
        vector<int>  w;
};     


typedef map<int, Word > Ass_fifo;

inline Word  get_fifo(const Ass_fifo &as, int var) {  
	Ass_fifo::const_iterator i = as.find(var);
	assert(i!=as.end());
        return (i->second); 
}

inline void insert_fifo(Ass_fifo &as, int var,int val) {   // a revoir?!!
	Ass_fifo::const_iterator it=as.find(var);
	if(it == as.end()) {
		if(val == -1)
			as[var]=Word();
		else
			as[var]=Word(val);
	}
	else {
		Word m;
		if(val == -1)
			m=Word();
		else
			m=Word(val);
		Word job(as[var]&m); 
		as.erase(var);
		as[var]=job; 
	}
	
}
inline size_t asHash(const Ass_fifo &as) { 
	size_t r = 1;
	for (Ass_fifo::const_iterator i=as.begin();i!=as.end();i++)
             r^=i->first^ (i->second).size();
	return r*9929;
}


#endif



