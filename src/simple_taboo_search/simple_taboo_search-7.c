#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "omp.h"
#include "fifo.h" /* for taboo list */
#include "graph_utils.h" /* for ReadGraph */

#define EDGEONLY

#define MAXSIZE (541)

#define TABOOSIZE (500)
#define BIGCOUNT (9999999)

/***
 *** example of very simple search for R(7,7) counter examples
 ***
 *** starts with a small randomized graph and works its way up to successively
 *** larger graphs one at a time
 ***
 *** uses a taboo list of size #TABOOSIZE# to hold and encoding of and edge
 *** (i,j)+clique_count
 ***/

int
main(int argc,char *argv[])
{
	int *g;
	int *new_g;
	int gsize;
	int count;
	int i;
	int j;
	int new_count;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;
	int globalBestCount = BIGCOUNT;
	int bcIncrease = 0;

#if 1
	/*
	 * start with graph of size 8
	 */
	gsize = 8;
	g = (int *)malloc(gsize*gsize*sizeof(int));
	if(g == NULL) {
		exit(1);
	}
#else
	/*
	 * start with pre-computed graph of size 109
	 */
	ReadGraph("../../counterexamples/n109.txt", &g, &gsize);
#endif

	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}

	/*
	 * start out with all zeros
	 */
	//memset(g,0,gsize*gsize*sizeof(int));

	/*
	 * while we do not have a publishable result
	 */
	while(gsize < 206)
	{
		/*
		 * find out how we are doing
		 */
		unsigned long int *ecounts = malloc(gsize*gsize*sizeof(unsigned long int));
		if(ecounts == NULL) {
			printf("ERROR: ran out of memory during malloc of ecounts!\n");
			exit(1);
		}
		count = CliqueCountAll(g,gsize,ecounts);

		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found! Number of nodes: %d\n", gsize);
			//PrintGraph(g,gsize);

			/* Save counterexample into a file */
			SaveGraph(g,gsize, "counterexamples");

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
			 /* Last row and column will have a balanced number of 0s and 1s */
				if(drand48() > 0.5) {
					new_g[i*(gsize+1) + gsize] = 1; // last column
					new_g[gsize*(gsize+1) + i] = 1; // last row
				} else {
					new_g[i*(gsize+1) + gsize] = 0; // last column
					new_g[gsize*(gsize+1) + i] = 0; // last row
				}
			}

			/*
			 * throw away the old graph and make new one the
			 * graph
			 */
			free(g);
			g = new_g;
			gsize = gsize+1;

			/*
			 * reset the taboo list for the new graph
			 */
			taboo_list = FIFOResetEdge(taboo_list);

			/* Reset best_count increase count */
			bcIncrease = 0;
			/*
			 * keep going
			 */
			continue;
		}

		/* If bcIncrease is greater than the taboo size, add some randomness in
		 * edge flipping to get out of local minimum.
		 */
		if(bcIncrease > TABOOSIZE) {
			//Randomize(&g, gsize);
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
		best_count = BIGCOUNT;
//#pragma omp parallel for private(i,j,count) shared(taboo_list,best_count,best_i,best_j)
		for(i=0; i < gsize; i++)
		{
			for(j=i+1; j < gsize; j++)
			{
				/*
				 * flip it
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];

				/*
				 * compute the new count based on the edge flip
				 */
				//count = CliqueCount(g,gsize);
				new_count = count - ecounts[i*gsize+j] + CliqueCountEdge(g,gsize,i,j);

				/*
				 * is it better and the i,j,count not taboo?
				 */
#ifdef EDGEONLY
				if((count < best_count) && 
					!FIFOFindEdge(taboo_list,i,j))
#else
				if((count < best_count) && 
					!FIFOFindEdgeCount(taboo_list,i,j,count))
#endif
				{
					best_count = count;
					best_i = i;
					best_j = j;
				}

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
			}
		}

		if(best_count == BIGCOUNT) {
			printf("no best edge found, terminating\n");
			exit(1);
		}
	
		/*
		 * keep the best flip we saw
		 */
		g[best_i*gsize+best_j] = 1 - g[best_i*gsize+best_j];

		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		count = CliqueCount(g,gsize);
#ifdef EDGEONLY
		FIFOInsertEdge(taboo_list,best_i,best_j);
#else
		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);
#endif

		printf("ce size: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
			gsize,
			best_count,
			best_i,
			best_j,
			g[best_i*gsize+best_j]);

		/* Update global best count  and save intermediate result in a file */
		if(best_count <= globalBestCount) {
			globalBestCount = best_count;
			SaveGraph(g,gsize, "intermediate");
		}
		/* If best_count is increasing, it may mean that we reached a local minimum.
		 * Keep track of how many times best_count increases in value
		 */
		else {
			bcIncrease++;
		}
		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);


	return(0);

}
