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
  int N;

  if (argc!=2) return(-1);
  if ((N=atoi(argv[1]))<1) return(-1);
  char nom[20];
  sprintf(nom,"kanban%d.net",N);
  ofstream f(nom);

  for(int i=4;i>0;i--){
    int  mod = i - 1;
    f<<"#place ("<<mod<<") Pm"<<i<<endl;
    f<<"#place ("<<mod<<") Pback"<<i<<endl;
    f<<"#place ("<<mod<<") Pout"<<i<<endl;
    f<<"#place ("<<mod<<") P"<<i<<" mk("<<N<<"*<..>)"<<endl;
  }

  f<<"#trans tin4"<<endl;
  f<<"in {P4:<..>;}"<<endl;
  f<<"out {Pm4:<..>;}"<<endl;
  f<<"#endtr"<<endl;

  for(int i=4;i>0;i--){    
    f<<"#trans tredo"<<i<<endl;
    f<<"in {Pm"<<i<<":<..>;}"<<endl;
    f<<"out {Pback"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tback"<<i<<endl;
    f<<"in {Pback"<<i<<":<..>;}"<<endl;
    f<<"out {Pm"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tok"<<i<<endl;
    f<<"in {Pm"<<i<<":<..>;}"<<endl;
    f<<"out {Pout"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;
  }

  f<<"#trans tsynch4_23"<<endl;
  f<<"in {Pout4:<..>;P2:<..>;P3:<..>;}"<<endl;
  f<<"out {P4:<..>;Pm2:<..>;Pm3:<..>;}"<<endl;
  f<<"#endtr"<<endl;

  f<<"#trans tsynch1_23"<<endl;
  f<<"in {Pout3:<..>;Pout2:<..>;P1:<..>;}"<<endl;
  f<<"out {P3:<..>;P2:<..>;Pm1:<..>;}"<<endl;
  f<<"#endtr"<<endl;

  f<<"#trans tout1"<<endl;
  f<<"in {Pout1:<..>;}"<<endl;
  f<<"out {P1:<..>;}"<<endl;
  f<<"#endtr"<<endl;

  cout<<flush;
  
}
