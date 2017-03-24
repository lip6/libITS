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

#ifndef __HOM_ARCS__H_
#define __HOM_ARCS__H_


/** This file contains declarations for the DDD homomorphisms that correspond 
 *  to basic Petri net arc function :
 *  PrePlace, PostPlace, InhibPlace
 *  CapacityPlace, ResetPlace
**/

#include "ddd/DDD.h"
#include "ddd/Hom.h"
#include "ddd/Hom_Basic.hh"

// Construct a Hom for a StrongHom _PostPlace
inline GHom postPlace(int pl, int post){
  return incVar(pl, post);
}


// Construct a Hom for a StrongHom _InhibitorPlace
inline GHom inhibitorPlace(int pl, int pre){
  return  varLtState(pl, pre);
}

// Construct a Hom for a StrongHom _PrePlace
inline GHom prePlace(int pl, int pre){
  return incVar(pl, -pre) & varGeqState(pl,pre);
}


// Construct a Hom for a place with capacity
inline GHom capacityPlace(int pl, int cap){
     return varLeqState(pl,cap);
}


inline GHom resetPlace(int pl){
  return setVarConst(pl,0);
}

#endif // __HOM_ARCS__H_


