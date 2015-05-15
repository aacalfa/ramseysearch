#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "clique_count.h"
#include "graph_utils.h"
#include "fifo.h"
#include "class_fifo.h"

extern PhoneHome();
int Best_g[170*170];
int Best_size;
int Best_so_far = 999999;

#define PHONEFIFOSIZE (10000)
#define FIFOSIZE (1000000)


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
	void *f;
	int new_gsize;

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
	 * make a fifo for the tabu list
	 */

	f = FIFOInitEdge(FIFOSIZE);
	Phone_FIFO = FIFOInit(PHONEFIFOSIZE);

	if(f == NULL)
		exit(1);

	/*
	 * while we do not have a publishable result
	 */
	while(gsize < 102)
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

			memset(Best_g,0,sizeof(Best_g));
			memcpy(Best_g,g,gsize*sizeof(int));
			Best_size = gsize;
			PhoneHome();
reset:
			new_gsize = NextPrime(Best_size);	

			/*
			 * make a new graph one size bigger
			 */
			new_g = 
			(int *)malloc((new_gsize)*(new_gsize)*sizeof(int));
			if(new_g == NULL)
				exit(1);
			/*
			 * copy the old graph into the new graph leaving the
			 * last row and last column alone
			 */
			CopyGraph(Best_g,Best_size,new_g,new_gsize);

			/*
			 * zero out the last column and last row
			 */
			for(i=0; i < new_gsize; i++)
			{
				new_g[i*new_gsize + 
					new_gsize] = 0; // last column
				new_g[new_gsize*new_gsize + i] = 0; // last row
			}

			/*
			 * throw away the old graph and make new one the
			 * graph
			 */
			free(g);
			g = new_g;
			gsize = Best_size;


			f = FIFOResetEdge(f);
			Phone_FIFO = FIFOReset(Phone_FIFO);

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
			 * is it better and not already tabu?
			 */
			if((count < best_count) &&
				!FIFOFindEdge(f,i,count))
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
		printf("size: %d best_count: %d, best distance: %d, color: %d\n",
			gsize,
			best_count,
			best_i,
			g[0*gsize+best_i]);

		/*
		 * add this edge to the fifo
		 *
		 * no need to insert if we found a CE on this trip
		 */
		if(best_count != 0)
		{
			FIFOInsertEdge(f,best_i,best_count);
			FIFOInsertGraphBits(Phone_FIFO,g,gsize);
			if(best_count < Best_so_far)
			{
				memcpy(Best_g,g,gsize*gsize*sizeof(int));
				Best_size = gsize;
				PhoneHome();
				if(Best_size > gsize)
				{
					goto reset;
				}
				count = CliqueCount(Best_g,Best_size);
				if(count < Best_so_far)
				{
					memcpy(g,Best_g,
					Best_size*Best_size*sizeof(int));
					Best_so_far = count;
				}

			}
		}

		if(FIFOCount(Phone_FIFO) == PHONEFIFOSIZE)
		{
			memcpy(Best_g,g,gsize*gsize*sizeof(int));
			Best_size = gsize;
			PhoneHome();
			if(Best_size > gsize)
			{
				goto reset;
			}
			count = CliqueCount(Best_g,Best_size);
			if(count < Best_so_far)
			{
				memcpy(g,Best_g,
				Best_size*Best_size*sizeof(int));
				Best_so_far = count;
			}
		}

		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteEdge(f);


	return(0);

}
