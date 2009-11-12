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
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;



int main(int argc,char **argv){	
  ofstream f("model.net");


  int  comp_limit [] = {1,3,5,7,9,11,13,15,17,19,21,23,25,27,33,39,45,57,59,62,64,69,72,75,77,79,81,83,85,87,89,91,93,95,97,99,101,103,105,107,109,112,115,118,119,120,121,122,123,124,126,128,158,188,218,248,278,308,338,368,398,428,442,444,446,449,451,466,481,483};
  int * curcomp = comp_limit;
  int curmod=0;
  for(int i=0; i<=484 ; i++){
    if (i==*curcomp) {
      curcomp++;
      curmod++;
    }
    f<<"#place ("<<curmod<<") p"<<i ;
    if (i == 0)
      f << "  mk(<..>)";
    f<<endl ;
  }
  
}
