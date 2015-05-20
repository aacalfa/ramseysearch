#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "client.h"
#include "utils.h"
#include "omp.h"
#include "fifo.h" /* for taboo list */
#include "graph_utils.h" /* for ReadGraph */
#include "clique_count.h"
#include "msg.h"

#define EDGEONLY

#define MAXSIZE (541)

#define TABOOSIZE (500)
#define BIGCOUNT (9999999)

#define RANDOM_FLIP_RATIO (20) /* Number of edges to be randomly flipped at once */
#define BCINCREASE_THRESHOLD (100) /* Attempts of decreasing the best count */
#define ITERATIONS_THRESHOLD (500) /* How many iterations will be done before randomizing */
#define COUNT_RATIO_THRESHOLD (5) /* Ratio: how good your current count compared to global BC */

static void Randomize(int *g, int gsize) {
	int e;
	for(e = 0; e < gsize * (gsize - 1) / RANDOM_FLIP_RATIO; e++) {
		int i = rand() % gsize;
		int j = rand() % gsize;
		g[i*gsize + j] = 1 - g[i*gsize + j];
	}
}

/***
 *** example of very simple search for R(7,7) counter examples
 ***
 *** starts with a small randomized graph and works its way up to successively
 *** larger graphs one at a time
 ***
 *** uses a taboo list of size #TABOOSIZE# to hold and encoding of and edge
 *** (i,j)+clique_count
 ***/

int tabooSearch(int *g, int matrixSize) /* when no matrix, input matrixSize as -1 */
{
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
    
    /* stubbornness parameters */
	int bcIncrease = 0;
    int iterations = 0;

	/* check whether it has a start point  */
	if(matrixSize <= 0) { /* if no, just start from 8 */
		gsize = 8;
		g = (int *)malloc(gsize*gsize*sizeof(int));
		if(g == NULL)
			exit(1);
		/*
		* start out with all zeros
		*/
		memset(g,0,gsize*gsize*sizeof(int));
	}else { /* if yes, start with the matrix */
		gsize = matrixSize;
	}
	
	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}
	
	while(gsize < 206)
	{
		/*
		 * find out how we are doing
		 */
		int *ecounts = malloc(gsize*gsize*sizeof(int));
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

			/* Save counterexample into a file */
			SaveGraph(g,gsize, "../../../counterexamples");

			/* Send counterexample to Server */
			int ret = sendResult(GraphtoChar(g, gsize), NumtoString(gsize), NumtoString(count));

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
					new_g[i*(gsize+1) + gsize] = 1; /* last column */
					new_g[gsize*(gsize+1) + i] = 1; /* last row */
				} else {
					new_g[i*(gsize+1) + gsize] = 0; /* last column */
					new_g[gsize*(gsize+1) + i] = 0; /* last row */
				}
			}

			/*
			 * throw away the old graph and make new one the
			 * graph
			 */
			free(ecounts);
			free(g);
			g = new_g;
			gsize = gsize+1;

			/*
			 * enlarge the edge clique count cache
			 */
			ecounts = malloc(gsize*gsize*sizeof(int));
			if(ecounts == NULL) {
				printf("ERROR: ran out of memory during malloc of ecounts!\n");
				exit(1);
			}

			/*
			 * reset the taboo list for the new graph
			 */
			taboo_list = FIFOResetEdge(taboo_list);

			/* Reset stubbornness parameters */
			bcIncrease = 0;
			iterations = 0;
			globalBestCount = BIGCOUNT;
            
			/*
			 * keep going
			 */
			continue;
		}

		/* If stubbornness parameters are met, add some randomness to help escape local min
		 */
		if(bcIncrease > BCINCREASE_THRESHOLD && (iterations > ITERATIONS_THRESHOLD || count > globalBestCount * COUNT_RATIO_THRESHOLD)) {
			printf("Stubbornness threshold reached with bcIncrease=%d, iterations=%d, count=%d, globalBestCount=%d\n", bcIncrease, iterations, count, globalBestCount);

			/* First, let's see if the server contains a counterexample for n greater than gsize */
			char *feedback = sendRequest(NumtoString(gsize));
			if(feedback != NULL) {
				if(feedback[0] != DENY) {
					/* Server has a counterexample with n greater than gsize */
					g = parseMessage(feedback, &gsize, &count);
					printf("Got a counterexample from server! New graphsize: %d\n", gsize);
				}
				/* Server does not have a better counterexample, randomize flips */
				else {
					Randomize(g, gsize);
				}
			}

			/*
			 * reset the taboo list for the new graph
			 */
			taboo_list = FIFOResetEdge(taboo_list);

			bcIncrease = 0;
			iterations = 0;
			globalBestCount = BIGCOUNT;
		}
		else {
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
					if((new_count < best_count) && 
						!FIFOFindEdge(taboo_list,i,j))
#else
					if((new_count < best_count) && 
						!FIFOFindEdgeCount(taboo_list,i,j,count))
#endif
					{
						best_count = new_count;
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

			printf("ce size: %d, best_count: %d, count: %d, best edge: (%d,%d), new color: %d\n",
				gsize,
				best_count,
				count,
				best_i,
				best_j,
				g[best_i*gsize+best_j]);

			/* Update global best count  and save intermediate result in a file */
			if(best_count < globalBestCount) {
				globalBestCount = best_count;
				SaveGraph(g,gsize, "../../../intermediate");
				bcIncrease = 0;
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
			iterations++;
		}
	}

	FIFODeleteGraph(taboo_list);

	return(0);

}
