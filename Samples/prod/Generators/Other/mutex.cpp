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
 


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int
main(int argc,char **argv)
{	
  int i,j,n;
  char Mutex[20]="Mutex";
  FILE *f;

  if (argc!=2) return(-1);
  if ((n=atoi(argv[1]))<2) return(-1);

  strcat(Mutex,argv[1]);
  strcat(Mutex,".net");

  if ((f=fopen(Mutex,"w"))==NULL)
    return (-1);


  fprintf(f,"#place Mutex  mk(<..>)\n");

  for(i=0;i<n;i++)
    {
      fprintf(f,"#place Idle%d mk(<..>)\n",i);
      fprintf(f,"#place Wait%d\n",i);
      fprintf(f,"#place Update%d\n",i);
      fprintf(f,"#place Mess%d\n",i);
      fprintf(f,"#place Ack%d\n",i);
    }

  for(i=0;i<n;i++)
    {


      fprintf(f,"#trans T1_%d\n",i);
      fprintf(f,"in {Idle%d:<..>;Mutex:<..>;}\n",i);
      fprintf(f,"out {Wait%d:<..>;",i);
      for(j=0;j<i;j++)
	fprintf(f,"Mess%d:<..>;",j);
      for(j=i+1;j<n;j++)
	fprintf(f,"Mess%d:<..>;",j);
      fprintf(f,"}\n");
      fprintf(f,"#endtr\n");

      fprintf(f,"#trans T2_%d\n",i);
      fprintf(f,"in {Wait%d:<..>;",i);
      for(j=0;j<i;j++)
	fprintf(f,"Ack%d:<..>;",j);
      for(j=i+1;j<n;j++)
	fprintf(f,"Ack%d:<..>;",j);
      fprintf(f,"}\n");
      fprintf(f,"out {Idle%d:<..>;Mutex:<..>;}\n",i);
      fprintf(f,"#endtr\n");

      fprintf(f,"#trans T3_%d\n",i);
      fprintf(f,"in {Idle%d:<..>;Mess%d:<..>;}\n",i,i);
      fprintf(f,"out {Update%d:<..>;}\n",i);
      fprintf(f,"#endtr\n");

      fprintf(f,"#trans T4_%d\n",i);
      fprintf(f,"in {Update%d:<..>;}\n",i);
      fprintf(f,"out {Idle%d:<..>;Ack%d:<..>;}\n",i,i);
      fprintf(f,"#endtr\n");
    }
  fclose(f);
}
