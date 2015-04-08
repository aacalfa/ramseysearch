#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "clique_count.h"
#include "graph_utils.h"

#define MAXSIZE (540)

/***
 *** example of very simple search for R(6,6) counter examples
 *** using symmetric graphs
 ***/


/*
 * flips the edge colors of all of the edges that are separated 
 * by #distance# number of nodes
 */
void
Recolor(int *g, int gsize, int distance)
{
        int i;
        int x;
        int y;


        for(i=0; i < gsize; i++)
        {
                x = i;
                y = (i+distance)%gsize;

                if(x < y)
                {
                        g[x*gsize+y] = 1 - g[x*gsize+y];
                }
                else
                {
                        g[y*gsize+x] = 1 - g[y*gsize+x];
                }
        }
        return;
}


/*
 * CopyGraph 
 *
 * copys the contents of old_g to corresponding locations in new_g
 * leaving other locations in new_g alone
 * that is
 * 	new_g[i,j] = old_g[i,j]
 */
void CopyGraph(int *old_g, int o_gsize, int *new_g, int n_gsize)
{
	int i;
	int j;

	/*
	 * new g must be bigger
	 */
	if(n_gsize < o_gsize)
		return;

	for(i=0; i < o_gsize; i++)
	{
		for(j=0; j < o_gsize; j++)
		{
			new_g[i*n_gsize+j] = old_g[i*o_gsize+j];
		}
	}

	return;
}

int
main(int argc,char *argv[])
{
	int *g;
	int *new_g;
	int gsize;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;

	/*
	 * start with graph of size 8
	 */
	gsize = 8;
	g = (int *)malloc(gsize*gsize*sizeof(int));
	if(g == NULL)
		exit(1);

	/*
	 * start out with all zeros
	 */
	memset(g,0,gsize*gsize*sizeof(int));

	/*
	 * while we do not have a publishable result
	 */
	while(gsize < 206)
	{
		/*
		 * find out how we are doing
		 */
		count = CliqueCount(g,gsize);

		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			/*
			 * make a new graph one size bigger
			 */
			new_g = (int *)malloc((gsize+1)*(gsize+1)*sizeof(int));
			if(new_g == NULL)
				exit(1);
			/*
			 * copy the old graph into the new graph leaving the
			 * last row and last column alone
			 */
			CopyGraph(g,gsize,new_g,gsize+1);

			/*
			 * zero out the last column and last row
			 */
			for(i=0; i < (gsize+1); i++)
			{
				new_g[i*(gsize+1) + gsize] = 0; // last column
				new_g[gsize*(gsize+1) + i] = 0; // last row
			}

			/*
			 * throw away the old graph and make new one the
			 * graph
			 */
			free(g);
			g = new_g;
			gsize = gsize+1;

			/*
			 * keep going
			 */
			continue;
		}

		/*
		 * otherwise, we need to consider flipping an edge
		 *
		 * let's speculative flip each edge, record the new count,
		 * and unflip the edge.  We'll then remember the best flip and
		 * keep it next time around
		 *
		 * only need to work with upper triangle of matrix =>
		 * notice the indices
		 */
		best_count = 9999999;
		for(i=1; i <= gsize/2; i++)
		{
			/*
			 * flip the edges corresponding to this distance
			 */
			Recolor(g,gsize,i);
			count = CliqueCount(g,gsize);

			/*
			 * is it better?
			 */
			if(count < best_count)
			{
				best_count = count;
				best_i = i;
			}

			/*
			 * flip it back
			 */
			Recolor(g,gsize,i);
		}
		
		/*
		 * keep the best flip we saw
		 */
		Recolor(g,gsize,best_i);
		printf("size: %d, best_count: %d, best distance: %d, color: %d\n",
			gsize,
			best_count,
			best_i,
			g[0*gsize+best_i]);

		/*
		 * rinse and repeat
		 */
	}


	return(0);

}
