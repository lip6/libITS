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

#include "DDD.h"
#include "Hom.h"
#include "Hom_Basic.hh"

// **********************************************
class _InhibitorPlace:public StrongHom {
 public:
  int pl;
 private:
  int pre;
public:
	_InhibitorPlace(int p, int pr) : pl(p), pre(pr) {}

    bool
    skip_variable(int var) const
    {
        return pl != var;
    }

	GDDD phiOne() const {
		return GDDD::one;
	}                   

	GHom phi(int vr, int vl) const {
		if (pl == vr)
		{
			if (vl < pre)
				return GHom(vr, vl, GHom::id);
			else
				return GHom(GDDD::null);
		}
		else
		{
		    return GHom(vr, vl, GHom(this)); 
	    }
	}

	size_t hash() const {
		return 8097*pl^pre;
	}

	bool operator==(const StrongHom &s) const {
		_InhibitorPlace* ps = (_InhibitorPlace*)&s;
		return pl == ps->pl && pre == ps->pre;
	}

  _GHom * clone () const {  return new _InhibitorPlace(*this); }

  void print (std::ostream & os) const {
    os << "Inh(" << DDD::getvarName(pl) << "," << pre << ")";
  }
};

// **********************************************
class _PrePlace:public StrongHom {
   public:
  int pl;
 private:
  int  pre;
public:
	_PrePlace(int p, int pr) : pl(p), pre(pr) {}

	GDDD phiOne() const {
		return GDDD::one;
	}                   

    bool
    skip_variable(int var) const
    {
        return pl != var;
    }
    

	GHom phi(int vr, int vl) const {
		if (pl == vr)
		{
			if (vl >= pre)
				return GHom(vr, vl - pre, GHom::id);
			else
				return GHom(GDDD::null);
		}
		else
		{
		    return GHom(vr, vl, GHom(this)); 
	    }
	}

	size_t hash() const {
		return 6619*pl^pre;
	}

	bool operator==(const StrongHom &s) const {
		_PrePlace* ps = (_PrePlace*)&s;
		return pl == ps->pl && pre == ps->pre;
	}

  _GHom * clone () const {  return new _PrePlace(*this); }

  void print (std::ostream & os) const {
    os << "Pre(" << DDD::getvarName(pl) << "," << pre << ")";
  }

};

// **********************************************
class _PostPlace:public StrongHom {
   public:
  int pl;
 private:
  int  post;
public:
	_PostPlace(int p, int po) : pl(p), post(po) {}

    bool
    skip_variable(int var) const
    {
        return pl != var;
    }
    

	GDDD phiOne() const {
		return GDDD::one;
	}                   

	GHom phi(int vr, int vl) const {
	  if (pl == vr) {
	    // Just a trick to detect if we are going unbounded ...
	    //	    if (vl > 1) std::cerr << "reached value " << vl << " for place " << DDD::getvarName(vr) << std::endl; 
	      return GHom(vr, vl + post, GHom::id);
	  }
	  return GHom(vr, vl, GHom(this)); 
	}

	size_t hash() const {
		return 9049*pl^post;
	}

	bool operator==(const StrongHom &s) const {
		_PostPlace* ps = (_PostPlace*)&s;
		return pl == ps->pl && post == ps->post;
	}

  _GHom * clone () const {  return new _PostPlace(*this); }

  void print (std::ostream & os) const {
    os << "Post(" << DDD::getvarName(pl) << "," << post << ")";
  }

};


// **********************************************
class _ResetPlace : public StrongHom {
public:
  int pl;
	_ResetPlace(int p):pl(p) {}

	GDDD phiOne() const {
		return GDDD::one;
	}                   

    bool
    skip_variable(int var) const
    {
        return pl != var;
    }
    

	GHom phi(int vr, int vl) const {
		if (pl == vr)
			return GHom(vr, 0, GHom::id);
		return GHom(vr, vl, GHom(this)); 
	}

	size_t hash() const {
		return 6011*pl;
	}

	bool operator==(const StrongHom &s) const {
		_ResetPlace* ps = (_ResetPlace*)&s;
		return pl == ps->pl;
	}

  _GHom * clone () const {  return new _ResetPlace(*this); }

  void print (std::ostream & os) const {
    os << "Reset(" << DDD::getvarName(pl) << ")";
  }

};



// Construct a Hom for a StrongHom _PostPlace
inline GHom postPlace(int pl, int post){
  return incVar(pl, post);
}


// Construct a Hom for a StrongHom _InhibitorPlace
inline GHom inhibitorPlace(int pl, int pre){
	return _InhibitorPlace(pl, pre);
}

// Construct a Hom for a StrongHom _PrePlace
inline GHom prePlace(int pl, int pre){
	return _PrePlace(pl, pre);
}


// Construct a Hom for a place with capacity
inline GHom capacityPlace(int pl, int cap){
	return _InhibitorPlace(pl, cap+1);
}


inline GHom resetPlace(int pl){
   return _ResetPlace(pl);
}

#endif // __HOM_ARCS__H_


