#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <strings.h>
#include <math.h>


#define sgsize (7)

/*
 ***
 *** returns the number of monochromatic cliques in the graph presented to
 *** it
 ***
 *** graph is stored in row-major order
 *** only checks values above diagonal
 */

int CliqueCount(int *g,
	     int gsize)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int count=0;
    
    for(i=0;i < gsize-sgsize+1; i++)
    {
	for(j=i+1;j < gsize-sgsize+2; j++)
        {
	    for(k=j+1;k < gsize-sgsize+3; k++) 
            { 
		if((g[i*gsize+j] == g[i*gsize+k]) && 
		   (g[i*gsize+j] == g[j*gsize+k]))
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    { 
			if((g[i*gsize+j] == g[i*gsize+l]) && 
			   (g[i*gsize+j] == g[j*gsize+l]) && 
			   (g[i*gsize+j] == g[k*gsize+l]))
			{
			    for(m=l+1;m < gsize-sgsize+5; m++) 
			    {
				if((g[i*gsize+j] == g[i*gsize+m]) && 
				   (g[i*gsize+j] == g[j*gsize+m]) &&
				   (g[i*gsize+j] == g[k*gsize+m]) && 
				   (g[i*gsize+j] == g[l*gsize+m]))
				{
				  for(n=m+1;n<gsize-sgsize+6;n++)
					{
					  if
				   ((g[i*gsize+j] == g[i*gsize+n]) && 
				   (g[i*gsize+j] == g[j*gsize+n]) &&
				   (g[i*gsize+j] == g[k*gsize+n]) && 
				   (g[i*gsize+j] == g[l*gsize+n]) &&
				   (g[i*gsize+j] == g[m*gsize+n]))
			    {
			          for(o=n+1;o<gsize-sgsize+7;o++) {
					if
				  ((g[i*gsize+j] == g[i*gsize+o]) &&
				  (g[i*gsize+j] == g[j*gsize+o]) &&
				  (g[i*gsize+j] == g[k*gsize+o]) &&
				  (g[i*gsize+j] == g[l*gsize+o]) &&
				  (g[i*gsize+j] == g[m*gsize+o]) &&
				  (g[i*gsize+j] == g[n*gsize+o])) {
			      		count++;
				  }
				}
			    }
					}
	
				}
			    }
			}
		    }
		}
	    }
         }
     }
    return(count);
}

double CliqueCountG(int *g, int gsize, double *countg)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    double count=0.0;

    /*
     * zero out countg
     */
    if(countg != NULL)
    {
	    memset(countg,0,gsize*gsize*sizeof(double));
    }
    
    for(i=0;i < gsize-sgsize+1; i++)
    {
	for(j=i+1;j < gsize-sgsize+2; j++)
        {
	    for(k=j+1;k < gsize-sgsize+3; k++) 
            { 
		if((g[i*gsize+j] == g[i*gsize+k]) && 
		   (g[i*gsize+j] == g[j*gsize+k]))
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    { 
			if((g[i*gsize+j] == g[i*gsize+l]) && 
			   (g[i*gsize+j] == g[j*gsize+l]) && 
			   (g[i*gsize+j] == g[k*gsize+l]))
			{
			    for(m=l+1;m < gsize-sgsize+5; m++) 
			    {
				if((g[i*gsize+j] == g[i*gsize+m]) && 
				   (g[i*gsize+j] == g[j*gsize+m]) &&
				   (g[i*gsize+j] == g[k*gsize+m]) && 
				   (g[i*gsize+j] == g[l*gsize+m]))
				{
				  for(n=m+1;n<gsize-sgsize+6;n++)
					{
					  if
				   ((g[i*gsize+j] == g[i*gsize+n]) && 
				   (g[i*gsize+j] == g[j*gsize+n]) &&
				   (g[i*gsize+j] == g[k*gsize+n]) && 
				   (g[i*gsize+j] == g[l*gsize+n]) &&
				   (g[i*gsize+j] == g[m*gsize+n])) {
				     for(o=n+1;o<gsize-sgsize+7;o++) {
					if
				   ((g[i*gsize+j] == g[i*gsize+o]) &&
				   (g[i*gsize+j] == g[j*gsize+o]) &&
				   (g[i*gsize+j] == g[k*gsize+o]) &&
				   (g[i*gsize+j] == g[l*gsize+o]) &&
				   (g[i*gsize+j] == g[m*gsize+o]) &&
				   (g[i*gsize+j] == g[n*gsize+o])) {
			      count++;
			      if(countg != NULL)
			      {
				      countg[i*gsize+j]++;  countg[i*gsize+k]++;
				      countg[i*gsize+l]++;  countg[i*gsize+m]++;
				      countg[i*gsize+n]++;  countg[j*gsize+k]++;
				      countg[j*gsize+l]++;  countg[j*gsize+m]++;
				      countg[j*gsize+n]++;  countg[k*gsize+l]++;
				      countg[k*gsize+m]++;  countg[k*gsize+n]++;
				      countg[l*gsize+m]++;  countg[l*gsize+n]++;
				      countg[m*gsize+n]++;  countg[i*gsize+o]++;
				      countg[j*gsize+o]++;  countg[k*gsize+o]++;
				      countg[l*gsize+o]++;  countg[m*gsize+o]++;
				      countg[n*gsize+o]++; 
			      }
			      }
			      }
			    }
					}
	
				}
			    }
			}
		    }
		}
	    }
         }
     }
    return(count);
}








