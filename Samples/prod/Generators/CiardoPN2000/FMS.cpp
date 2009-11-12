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
  if ((N=atoi(argv[1]))<2) return(-1);
  char nom[20];
  sprintf(nom,"fms%d.net",N);
  ofstream f(nom);

  {
    f<<"#place (0)  P1 mk("<<N<<"*<..>)"<<endl;
    f<<"#place (0)  P1wM1"<<endl;
    f<<"#place (0)  P1M1"<<endl;
    f<<"#place (0)  M1 mk(3*<..>)"<<endl;
    f<<"#place (0)  P1d"<<endl;
    f<<"#place (0)  P1s"<<endl;
    f<<"#place (0)  P1wP2"<<endl;
    f<<"#place (2)  P12"<<endl;
    f<<"#place (2)  P12wM3"<<endl;
    f<<"#place (2)  P12M3"<<endl;
    f<<"#place (2)  M3 mk(2*<..>)"<<endl;
    f<<"#place (2)  P12s"<<endl;
    f<<"#place (1)  P2 mk("<<N<<"*<..>)"<<endl;
    f<<"#place (1)  P2wM2"<<endl;
    f<<"#place (1)  P2M2"<<endl;
    f<<"#place (1)  M2 mk(<..>)"<<endl;
    f<<"#place (1)  P2d"<<endl;
    f<<"#place (1)  P2s"<<endl;
    f<<"#place (1)  P2wP1"<<endl;
    f<<"#place (3)  P3 mk("<<N<<"*<..>)"<<endl;
    f<<"#place (3)  P3M2"<<endl;
    f<<"#place (3)  P3s"<<endl;
  }

  {    
    f<<"#trans tP1"<<endl;
    f<<"in {P1:<..>;}"<<endl;
    f<<"out {P1wM1:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tM1"<<endl;
    f<<"in {P1wM1:<..>;M1:<..>;}"<<endl;
    f<<"out {P1M1:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP1M1"<<endl;
    f<<"in {P1M1:<..>;}"<<endl;
    f<<"out {M1:<..>;P1d:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP1e"<<endl;
    f<<"in {P1d:<..>;}"<<endl;
    f<<"out {P1s:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP1s"<<endl;
    f<<"in {P1s:<..>;}"<<endl;
    f<<"out {P1:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP1j"<<endl;
    f<<"in {P1d:<..>;}"<<endl;
    f<<"out {P1wP2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tx"<<endl;
    f<<"in {P1wP2:<..>;P2wP1:<..>;}"<<endl;
    f<<"out {P12:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP12"<<endl;
    f<<"in {P12:<..>;}"<<endl;
    f<<"out {P12wM3:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tM3"<<endl;
    f<<"in {P12wM3:<..>;M3:<..>;}"<<endl;
    f<<"out {P12M3:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP12M3"<<endl;
    f<<"in {P12M3:<..>;}"<<endl;
    f<<"out {P12s:<..>;M3:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP12s"<<endl;
    f<<"in {P12s:<..>;}"<<endl;
    f<<"out {P1:<..>;P2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP2"<<endl;
    f<<"in {P2:<..>;}"<<endl;
    f<<"out {P2wM2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tM2"<<endl;
    f<<"in {P2wM2:<..>;M2:<..>;}"<<endl;
    f<<"out {P2M2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP2M2"<<endl;
    f<<"in {P2M2:<..>;}"<<endl;
    f<<"out {P2d:<..>;M2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP2j"<<endl;
    f<<"in {P2d:<..>;}"<<endl;
    f<<"out {P2wP1:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP2e"<<endl;
    f<<"in {P2d:<..>;}"<<endl;
    f<<"out {P2s:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP2s"<<endl;
    f<<"in {P2s:<..>;}"<<endl;
    f<<"out {P2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP3"<<endl;
    f<<"in {P3:<..>;}"<<endl;
    f<<"out {P3M2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP3M2"<<endl;
    f<<"in {P3M2:<..>;M2:<..>;}"<<endl;
    f<<"out {P3s:<..>;M2:<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans tP3s"<<endl;
    f<<"in {P3s:<..>;}"<<endl;
    f<<"out {P3:<..>;}"<<endl;
    f<<"#endtr"<<endl;
    f<<flush;
  }
}
