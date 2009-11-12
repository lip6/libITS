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
 


#include <cstring>
#include <cstdlib>
#include <cstdio>

int
main(int argc,char **argv)
{	
  int i,n;
  char Philo[20]="Philo";
  FILE *f;

  if (argc!=2) return(-1);
  if ((n=atoi(argv[1]))<2) return(-1);

  strcat(Philo,argv[1]);
  strcat(Philo,".net");

  if ((f=fopen(Philo,"w"))==NULL)
    return (-1);

  for(i=0;i<n;i++)
    {
      fprintf(f,"#place Think%d mk(<..>)\n",i);
      fprintf(f,"#place Eat%d\n",i);
      fprintf(f,"#place Fork%d mk(<..>)\n",i);
    }

  for(i=0;i<n;i++)
    {
      fprintf(f,"#trans TakeFork%d\n",i);
      fprintf(f,"in {Fork%d:<..>;Think%d:<..>;Fork%d:<..>;}\n",(i+n-1)%n,i,i);
      fprintf(f,"out {Eat%d:<..>;}\n",i);
      fprintf(f,"#endtr\n");

      fprintf(f,"#trans LeaveFork%d\n",i);
      fprintf(f,"in {Eat%d:<..>;}\n",i);
      fprintf(f,"out {Fork%d:<..>;Think%d:<..>;Fork%d:<..>;}\n",(i+n-1)%n,i,i);
      fprintf(f,"#endtr\n");
    }
  fclose(f);
}
