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
#include <cstring>
using namespace std;

int main(int argc,char **argv){	
  int N;
  bool interlace = false;
  if (argc > 2)
    interlace = true;
  if ((N=atoi(argv[1]))<2) return(-1);
  char nom[20];
  sprintf(nom,"ring%d.net",N);
  ofstream f(nom);

  for(int i=0;i<N;i++){
    int mod =  interlace ? ((2*i < N) ? 2*i : 1+2*(N-1-i)) : i  ;
    f<<"#place ("<<mod<<") C"<<i<<" mk(<..>)"<<endl;
    f<<"#place ("<<mod<<") E"<<i<<endl;
    f<<"#place ("<<mod<<") A"<<i<<" mk(<..>)"<<endl;
    f<<"#place ("<<mod<<") B"<<i<<endl;
    f<<"#place ("<<mod<<") D"<<i<<endl;
    f<<"#place ("<<mod<<") F"<<i<<endl;
    f<<"#place ("<<mod<<") G"<<i<<endl;
    f<<"#place ("<<mod<<") H"<<i<<endl;
  }

  for(int i=0;i<N;i++){
    
    f<<"#trans Free"<<i<<endl;
    f<<"in {A"<<i<<":<..>;C"<<((i+N-1)%N)<<":<..>;}"<<endl;
    f<<"out {B"<<i<<":<..>;E"<<((i+N-1)%N)<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Used"<<i<<endl;
    f<<"in {H"<<i<<":<..>;C"<<((i+N-1)%N)<<":<..>;}"<<endl;
    f<<"out {B"<<i<<":<..>;D"<<((i+N-1)%N)<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Get"<<i<<endl;
    f<<"in {F"<<i<<":<..>;B"<<i<<":<..>;}"<<endl;
    f<<"out {C"<<i<<":<..>;A"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Put"<<i<<endl;
    f<<"in {B"<<i<<":<..>;G"<<i<<":<..>;}"<<endl;
    f<<"out {C"<<i<<":<..>;H"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Other"<<i<<endl;
    f<<"in {D"<<i<<":<..>;}"<<endl;
    f<<"out {G"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Owner"<<i<<endl;
    f<<"in {D"<<i<<":<..>;}"<<endl;
    f<<"out {E"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans GoOn"<<i<<endl;
    f<<"in {E"<<i<<":<..>;}"<<endl;
    f<<"out {F"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Write"<<i<<endl;
    f<<"in {E"<<i<<":<..>;}"<<endl;
    f<<"out {G"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<flush;
  }
}
