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
  bool doInterlace = false;
  bool doHierarchy = false;

  fprintf(stderr,"Usage : Mutex N [-r/-h] \n N : number of process \n -r : activate poor interlaced ordering\n -h : activate hierarchy (not interlaced)\n");

  if (argc < 2) return(-1);
  if ((n=atoi(argv[1]))<2) return(-1);
  if (argc == 3) 
  {    
      if ( ! strcmp(argv[2],"-r") ) {
          strcat(Mutex,"_reo");
          doInterlace = true;
      } 
      else if ( ! strcmp(argv[2],"-h") ) {
          strcat(Mutex,"_hie");
          doHierarchy = true;
      }
  }
  strcat(Mutex,argv[1]);
  strcat(Mutex,".net");

  if ((f=fopen(Mutex,"w"))==NULL)
    return (-1);


  fprintf(f,"#place (0) p4 \n");

  if (! doInterlace) {
    if (! doHierarchy)
      for(i=0;i<n;i++)
	{
	  fprintf(f,"#place p1%c mk(<..>)\n",('A'+i));
	  fprintf(f,"#place p2%c \n",('A'+i));
	  fprintf(f,"#place p3%c \n",('A'+i));
	}
    else 
      for(i=0;i<n;i++)
	{
	  fprintf(f,"#place (%d) p1%c mk(<..>)\n",i+1,('A'+i));
	  fprintf(f,"#place (%d) p2%c \n",i+1,('A'+i));
	  fprintf(f,"#place (%d) p3%c \n",i+1,('A'+i));
	}


  } else {
    for(i=0;i<n;i++)
      fprintf(f,"#place p1%c mk(<..>)\n",('A'+i));
    for(i=0;i<n;i++)
      fprintf(f,"#place p2%c \n",('A'+i));
    for(i=0;i<n;i++)
      fprintf(f,"#place p3%c \n",('A'+i));
  }

  for(i=0;i<n;i++)
    {            
      fprintf(f,"#trans T1_%c\n",('A'+i));
      fprintf(f,"in {p1%c:<..>;}\n",('A'+i));
      fprintf(f,"out {p2%c:<..>;",('A'+i));
      fprintf(f,"}\n");
      fprintf(f,"#endtr\n");

      fprintf(f,"#trans T2_%c\n",('A'+i));
      fprintf(f,"in {p2%c:<..>;}\n",('A'+i));
      fprintf(f,"out {p3%c:<..>;",('A'+i));
      fprintf(f,"}\n");
      fprintf(f,"#endtr\n");
      
      fprintf(f,"#trans T3_%c\n",('A'+i));
      fprintf(f,"in {p3%c:<..>;",('A'+i));
      for (j=0; j <n; j++) 
	if (j != i)
	  fprintf(f,"p1%c:<..>;",('A'+j));
      fprintf(f,"}\n");
      fprintf(f,"out {p4:<..>;");
      fprintf(f,"}\n");
      fprintf(f,"#endtr\n");
    }

  fprintf (f,"#trans T4\n");
  fprintf(f,"in {p4:<..>;}\n");
  fprintf(f,"out {");
  for (j=0; j <n; j++) 
    fprintf(f,"p1%c:<..>;",('A'+j));
  fprintf(f,"}\n");
  fprintf(f,"#endtr\n");

  fclose(f);
}
