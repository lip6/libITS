
#include <stdio.h>
#include <string.h>

#define plus(x) ((x+1) % n)

int	main(int argc, char **argv)
{	int n = 0;

	if (argc != 2) return(-1);
	/*lecture du nombre de process */
	sscanf(argv[1], "%d", &n);
	if (n < 2) 
		return -1; 
	char name[256];
	sprintf(name, "peter%d.net", n);
	FILE* f = fopen(name, "w");
	if (!f) 
		return -1;

/**************************/	
/* declaration des places */	
/**************************/

	int	i, j, k;
	for(i = 0; i < n; ++i) {
		fprintf(f, "#place (%d) Init_%d mk (<..>) \n", i,i);
		for(j = 2;j < n; ++j)
			fprintf(f, "#place (%d) A_%d_%d \n", i,i, j);
		for(j = 1; j < n; ++j)
			fprintf(f, "#place (%d) B_%d_%d \n", i,i, j);
		for(j = 1; j < n; ++j)
			for(k = 0; k < n; ++k)
				if(i!=k)
					fprintf(f, "#place (%d) C_%d_%d_%d \n", i,i, j, k);
		fprintf(f, "#place (%d) SC_%d \n", i,i);
		fprintf(f, "#place (%d) Q_%d_0 mk(<..>)\n", i,i);
		for(j = 1; j < n; ++j)
			fprintf(f, "#place (%d) Q_%d_%d \n", i,i, j);
		if (i != 0) {
			fprintf(f, "#place (%d) Turn_%d_0 mk(<..>)\n", i,i);
			for(j = 1; j < n; ++j)
				fprintf(f, "#place (%d) Turn_%d_%d \n", i,i, j);
		}
	}

/*******************************/
/* declaration des transitions */
/*******************************/

/* start */
	{	int p;
		for(p=0;p<n;p++)
		{		fprintf(f, "#trans start_%d\n",p);
				fprintf(f, "in  { Init_%d:<..>;Q_%d_0:<..>;}\n",p,p);
				fprintf(f, "out { B_%d_1:<..>;Q_%d_1:<..>;}\n",p,p);
				fprintf(f, "#endtr\n");
		}
	}
/* ab */
	if (n>2)
	{	int p,j;
		for(p=0;p<n;p++)
		for(j=2;j<n;j++)
		{		fprintf(f, "#trans ab_%d_%d\n",p,j);
				fprintf(f, "in  { A_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,j,p,j-1);
				fprintf(f, "out { B_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,j,p,j);
				fprintf(f, "#endtr\n");
		}
	}
/* bc */
	{	int p,q,j;
		for(p=0;p<n;p++)
		for(q=0;q<n;q++)
		for(j=1;j<n;j++)
		{		fprintf(f, "#trans bc_%d_%d_%d\n",p,j,q);
				fprintf(f, "in  { B_%d_%d:<..>;Turn_%d_%d:<..>;}\n",p,j,j,q);
				fprintf(f, "out { C_%d_%d_%d:<..>;Turn_%d_%d:<..>;}\n",p,j,plus(p),j,p);
				fprintf(f, "#endtr\n");
		}
	}
/* cc */
	{	int p,q,x,j;
		for(p=0;p<n;p++)
		for(q=0;q<n;q++)
		for(j=1;j<n;j++)
		for(x=0;x<j;x++)
		if ((plus(q)!=p)&&(p!=q))
		{		fprintf(f, "#trans cc_%d_%d_%d_%d\n",p,j,q,x);
				fprintf(f, "in  { C_%d_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,j,q,q,x);
				fprintf(f, "out { C_%d_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,j,plus(q),q,x);
				fprintf(f, "#endtr\n");
		}
	}
	
/* ca */
	if (n>2)
	{	int p,j,q,x;
		for(q=0;q<n;q++)
		for(j=1;j<n-1;j++)
		for(x=0;x<j;x++)
		{		p=plus(q);
				fprintf(f, "#trans cc_%d_%d_%d_%d\n",p,j,q,x);
				fprintf(f, "in  { C_%d_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,j,q,q,x);
				fprintf(f, "out { A_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,j+1,q,x);
				fprintf(f, "#endtr\n");
		}
	}
	
/* ccs */
	{	int p,q,x;
		for(q=0;q<n;q++)
		for(x=0;x<n-1;x++)
		{		p=plus(q);
				fprintf(f, "#trans ccs_%d_%d_%d\n",p,q,x);
				fprintf(f, "in  { C_%d_%d_%d:<..>;Q_%d_%d:<..>;}\n",p,n-1,q,q,x);
				fprintf(f, "out { SC_%d:<..>;Q_%d_%d:<..>;}\n",p,q,x);
				fprintf(f, "#endtr\n");
		}
	}
	
/* caturn */
	if (n>2)
	{	int p,q,j,x;
		for(p=0;p<n;p++)
		for(q=0;q<n;q++)
		for(j=1;j<n-1;j++)
		for(x=0;x<n;x++)
		if ((x!=p)&&(p!=q))
		{		fprintf(f, "#trans caturn_%d_%d_%d_%d\n",p,j,q,x);
				fprintf(f, "in  { C_%d_%d_%d:<..>;Turn_%d_%d:<..>;}\n",p,j,q,j,x);
				fprintf(f, "out { A_%d_%d:<..>;Turn_%d_%d:<..>;}\n",p,j+1,j,x);
				fprintf(f, "#endtr\n");
		}
	}
	
/* ccsturn */
	{	int p,q,x;
		for(p=0;p<n;p++)
		for(q=0;q<n;q++)
		for(x=0;x<n;x++)
		if ((x!=p)&&(p!=q))
		{		fprintf(f, "#trans ccsturn_%d_%d_%d\n",p,q,x);
				fprintf(f, "in  { C_%d_%d_%d:<..>;Turn_%d_%d:<..>;}\n",p,n-1,q,n-1,x);
				fprintf(f, "out { SC_%d:<..>;Turn_%d_%d:<..>;}\n",p,n-1,x);
				fprintf(f, "#endtr\n");
		}
	}
	
/* csinit */
	{	int p;
		for(p=0;p<n;p++)
		{		fprintf(f, "#trans csinit_%d\n",p);
				fprintf(f, "in  { SC_%d:<..>;Q_%d_%d:<..>;}\n",p,p,n-1);
				fprintf(f, "out { Init_%d:<..>;Q_%d_%d:<..>;}\n",p,p,0);
				fprintf(f, "#endtr\n");
		}
	}
	fclose(f);



	
/* fin */	
}




