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
#ifndef RdPMONTEUR_H
#define RdPMONTEUR_H
#include <string>
#include <iostream>
using namespace std;

class RdPMonteur {
public:
  RdPMonteur(){};
  virtual ~RdPMonteur(){};
  virtual bool addPlace(const string &place,int marking=0, int capacity=0,int mod=0) =0;
  virtual bool addQueue(const string &place,int capacity=0,int mod =0) =0;
  virtual bool addLossQueue(const string &place,int capacity=0,int mod =0) =0;
  virtual bool addTrans(const string &transition) =0;
  virtual bool addPre(const string &place,const string &transition,int valuation=1) =0;
  virtual bool addPost(const string &place,const string &transition,int valuation=1) =0;  
  virtual bool addPreQueue(const string &place,const string &transition,int valuation=1) =0;
  virtual bool addPostQueue(const string &place,const string &transition,int valuation=1) =0;  
  virtual bool addInhibitor(const string &place,const string &transition,int valuation=1) =0;
  virtual bool addPreAuto(const string &place,const string &transition,const string &valuation) =0;
  virtual bool addPostAuto(const string &place,const string &transition,const string &valuation) =0;  
  virtual bool addReset(const string &place,const string &transition) =0;
  virtual bool create(const char *file);
};
#endif
