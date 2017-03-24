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
#ifndef MONTEUR_EXPRESSION_H
#define MONTEUR_EXPRESSION_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "its/expression/Word.hpp"
#include "its/expression/expression.hpp"
#include "its/parser_RdPE/RdPE.h"

typedef vector<Expression> Exp_map;
enum Rule_type{ ordinary, word ,number};

struct Terme{
	Rule_type  type;
	Expression *expression;
	int nb;
};
class Expression_Monteur {
public:
	Expression_Monteur (){ }
	bool create(const char *file,const RdPE& R);
	Exp_map Container;
};

#endif

