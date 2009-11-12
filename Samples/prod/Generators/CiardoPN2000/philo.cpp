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

int
main(int argc,char **argv)
{	
  int N;
  bool interlace = false;
  if (argc<2) return(-1);
  if ((N=atoi(argv[1]))<2) return(-1);
  if (argc > 2)
    interlace = true;
  char nom[20];
  sprintf(nom,"philo%d.net",N);
  ofstream f(nom);

  for(int i=0;i<N;i++){
    // int mod = i/2;
    int mod =  interlace ? ((2*i < N) ? 2*i : 1+2*(N-1-i)) : i  ;
    f<<"#place ("<<mod<<") Idle"<<i<< " mk(<..>)"<<endl;
    f<<"#place ("<<mod<<") WaitLeft"<<i<<endl;
    f<<"#place ("<<mod<<") HasLeft"<<i<<endl;
    f<<"#place ("<<mod<<") WaitRight"<<i<<endl;
    f<<"#place ("<<mod<<") HasRight"<<i<<endl;
    f<<"#place ("<<mod<<") Fork"<<i<<" mk(<..>)"<<endl;
  }

  for(int i=0;i<N;i++){
    f<<"#trans GoEat"<<i<<endl;
    f<<"in {Idle"<<i<<":<..>;}"<<endl;
    f<<"out {WaitLeft"<<i<<":<..>;WaitRight"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans GoLeft"<<i<<endl;
    f<<"in {Fork"<<((i+N-1)%N)<<":<..>;WaitLeft"<<i<<":<..>;}"<<endl;
    f<<"out {HasLeft"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans GoRight"<<i<<endl;
    f<<"in {Fork"<<i<<":<..>;WaitRight"<<i<<":<..>;}"<<endl;
    f<<"out {HasRight"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;

    f<<"#trans Release"<<i<<endl;
    f<<"in {HasLeft"<<i<<":<..>;HasRight"<<i<<":<..>;}"<<endl;
    f<<"out {Idle"<<i<<":<..>;Fork"<<((i+N-1)%N)<<":<..>;Fork"<<i<<":<..>;}"<<endl;
    f<<"#endtr"<<endl;
  }
  cout<<flush;
}
