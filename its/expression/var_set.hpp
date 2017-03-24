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
#ifndef _SET_
#define _SET_

#include <set>
using namespace std;

typedef set<int> IntSet;

inline void insert(IntSet &is, int v) { 
	is.insert(is.end(),v); 
}

inline bool belongs(const IntSet &is, int v) {
	return is.find(v)!=is.end();
}
inline ostream& operator<< (ostream& o, const IntSet &is) {
	for(IntSet::const_iterator it=is.begin(); it !=is.end(); it++) 
		o << *it << '.';
	return o;
}
#endif
