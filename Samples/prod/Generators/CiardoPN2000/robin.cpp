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
  bool interlace = false;
  if (argc > 2)
    interlace = true;
  if ((N=atoi(argv[1]))<2) return(-1);
  char nom[20];
  sprintf(nom,"robin%d.net",N);
  ofstream f(nom);

  f<<"#place ("<<0<<") Res"<<" mk(<..>)"<<endl;
  for(int i=1;i<=N;i++){
    int mod =  interlace ? ((2*i < N) ? 2*i : 1+2*(N-1-i)) : i  ;
    f<<"#place ("<<mod<<") R"<<i<<endl;
    f<<"#place ("<<mod<<") bufidle"<<i<<" mk(<..>)"<<endl;
    f<<"#place ("<<mod<<") buffull"<<i<<endl;
    if (i==1) {
      f<<"#place ("<<mod<<") pwait"<<i<<endl;
      f<<"#place ("<<mod<<") pask"<<i<<" mk(<..>)"<<endl;
    } else {
      f<<"#place ("<<mod<<") pwait"<<i<<" mk(<..>)"<<endl;
      f<<"#place ("<<mod<<") pask"<<i<<endl;
    }
    f<<"#place ("<<mod<<") pok"<<i<<endl;
    f<<"#place ("<<mod<<") pload"<<i<<endl;
    f<<"#place ("<<mod<<") psend"<<i<<endl;
  }



  for(int i=1;i<=N;i++){
    
//	Res:task[i], pask[i]:task[i], task[i]:R[i], task[i]:pok[i],
    f<<"#trans task"<<i<<endl;
    f<<"in {pask"<<i<<":<..>;Res:<..>;}"<<endl;
    f<<"out {R"<<i<<":<..>;pok"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

//	R[i]:tbuf[i], bufidle[i]:tbuf[i], tbuf[i]:buffull[i], tbuf[i]:Res,
    f<<"#trans tbuf"<<i<<endl;
    f<<"in {R"<<i<<":<..>;bufidle"<<i<<":<..>;}"<<endl;
    f<<"out {buffull"<<i<<":<..>;Res:<..>;}"<<endl;
    f<<"#endtr"<<endl;

//    	buffull[i]:t1load[i], pok[i]:t1load[i], t1load[i]:bufidle[i], t1load[i]:psend[i],
    f<<"#trans t1load"<<i<<endl;
    f<<"in {buffull"<<i<<":<..>;pok"<<i<<":<..>;}"<<endl;
    f<<"out {bufidle"<<i<<":<..>;psend"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;


//	buffull[i]:t2load[i], pload[i]:t2load[i], t2load[i]:bufidle[i], t2load[i]:pwait[i],
    f<<"#trans t2load"<<i<<endl;
    f<<"in {buffull"<<i<<":<..>;pload"<<i<<":<..>;}"<<endl;
    f<<"out {bufidle"<<i<<":<..>;pwait"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

//	pok[i]:t1send[i], pwait[mod(i+1,N)]:t1send[i], t1send[i]:pload[i], t1send[i]:pask[mod(i+1,N)],
    f<<"#trans t1send"<<i<<endl;
    f<<"in {pok"<<i<<":<..>;pwait"<<(1+ (i%N))<<":<..>;}"<<endl;
    f<<"out {pload"<<i<<":<..>;pask"<<(1+ (i%N))<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

//	psend[i]:t2send[i], pwait[mod(i+1,N)]:t2send[i], t2send[i]:pwait[i], t2send[i]:pask[mod(i+1,N)]
    f<<"#trans t2send"<<i<<endl;
    f<<"in {psend"<<i<<":<..>;pwait"<<(1+(i%N))<<":<..>;}"<<endl;
    f<<"out {pwait"<<i<<":<..>;pask"<<(1+(i%N))<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<flush;
  }
}
