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
                                        if((g[i*gsize+j] == g[i*gsize+n]) && 
                                           (g[i*gsize+j] == g[j*gsize+n]) &&
                                           (g[i*gsize+j] == g[k*gsize+n]) && 
                                           (g[i*gsize+j] == g[l*gsize+n]) &&
                                           (g[i*gsize+j] == g[m*gsize+n]))
                                        {
                                            for(o=n+1;o<gsize-sgsize+7;o++) 
                                            {
                                                if((g[i*gsize+j] == g[i*gsize+o]) &&
                                                   (g[i*gsize+j] == g[j*gsize+o]) &&
                                                   (g[i*gsize+j] == g[k*gsize+o]) &&
                                                   (g[i*gsize+j] == g[l*gsize+o]) &&
                                                   (g[i*gsize+j] == g[m*gsize+o]) &&
                                                   (g[i*gsize+j] == g[n*gsize+o])) 
                                                {
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

int CliqueCountEdge(int *g,
         int gsize, int i, int j)
{
    int k;
    int l;
    int m;
    int n;
    int o;
    int count=0;
    
    for(k=0;k < gsize-sgsize+3; k++) 
    { 
        if((k != i) && (k != j) &&
           (g[i*gsize+j] == g[i*gsize+k]) && 
           (g[i*gsize+j] == g[j*gsize+k]))
        {
            for(l=k+1;l < gsize-sgsize+4; l++) 
            { 
                if((l != i) && (l != j) &&
                   (g[i*gsize+j] == g[i*gsize+l]) && 
                   (g[i*gsize+j] == g[j*gsize+l]) && 
                   (g[i*gsize+j] == g[k*gsize+l]))
                {
                    for(m=l+1;m < gsize-sgsize+5; m++) 
                    {
                        if((m != i) && (m != j) &&
                           (g[i*gsize+j] == g[i*gsize+m]) && 
                           (g[i*gsize+j] == g[j*gsize+m]) &&
                           (g[i*gsize+j] == g[k*gsize+m]) && 
                           (g[i*gsize+j] == g[l*gsize+m]))
                        {
                            for(n=m+1;n<gsize-sgsize+6;n++)
                            {
                                if((n != i) && (n != j) &&
                                   (g[i*gsize+j] == g[i*gsize+n]) && 
                                   (g[i*gsize+j] == g[j*gsize+n]) &&
                                   (g[i*gsize+j] == g[k*gsize+n]) && 
                                   (g[i*gsize+j] == g[l*gsize+n]) &&
                                   (g[i*gsize+j] == g[m*gsize+n]))
                                {
                                    for(o=n+1;o<gsize-sgsize+7;o++) 
                                    {
                                        if((o != i) && (o != j) &&
                                           (g[i*gsize+j] == g[i*gsize+o]) &&
                                           (g[i*gsize+j] == g[j*gsize+o]) &&
                                           (g[i*gsize+j] == g[k*gsize+o]) &&
                                           (g[i*gsize+j] == g[l*gsize+o]) &&
                                           (g[i*gsize+j] == g[m*gsize+o]) &&
                                           (g[i*gsize+j] == g[n*gsize+o])) 
                                        {
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

    return(count);
}

int CliqueCountAll(int *g,
         int gsize, int *ecounts)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int count=0;

    if(ecounts != NULL)
    {
        memset(ecounts,0,gsize*gsize*sizeof(int));
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
                                        if((g[i*gsize+j] == g[i*gsize+n]) && 
                                           (g[i*gsize+j] == g[j*gsize+n]) &&
                                           (g[i*gsize+j] == g[k*gsize+n]) && 
                                           (g[i*gsize+j] == g[l*gsize+n]) &&
                                           (g[i*gsize+j] == g[m*gsize+n]))
                                        {
                                            for(o=n+1;o<gsize-sgsize+7;o++) 
                                            {
                                                if((g[i*gsize+j] == g[i*gsize+o]) &&
                                                   (g[i*gsize+j] == g[j*gsize+o]) &&
                                                   (g[i*gsize+j] == g[k*gsize+o]) &&
                                                   (g[i*gsize+j] == g[l*gsize+o]) &&
                                                   (g[i*gsize+j] == g[m*gsize+o]) &&
                                                   (g[i*gsize+j] == g[n*gsize+o])) 
                                                {
                                                    count++;

                                                    //update individual edge counts with found clique
                                                    if(ecounts != NULL)
                                                    {
                                                        ecounts[i*gsize+j]++;  ecounts[i*gsize+k]++;
                                                        ecounts[i*gsize+l]++;  ecounts[i*gsize+m]++;
                                                        ecounts[i*gsize+n]++;  ecounts[j*gsize+k]++;
                                                        ecounts[j*gsize+l]++;  ecounts[j*gsize+m]++;
                                                        ecounts[j*gsize+n]++;  ecounts[k*gsize+l]++;
                                                        ecounts[k*gsize+m]++;  ecounts[k*gsize+n]++;
                                                        ecounts[l*gsize+m]++;  ecounts[l*gsize+n]++;
                                                        ecounts[m*gsize+n]++;  ecounts[i*gsize+o]++;
                                                        ecounts[j*gsize+o]++;  ecounts[k*gsize+o]++;
                                                        ecounts[l*gsize+o]++;  ecounts[m*gsize+o]++;
                                                        ecounts[n*gsize+o]++; 
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
