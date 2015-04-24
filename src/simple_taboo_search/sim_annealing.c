#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "fifo.h" /* for taboo list */
#include "graph_utils.h" /* for ReadGraph */
#include "clique_count.h"

#define TABOOSIZE (500)
#define BIGCOUNT (9999999)

int RandRange(int Min, int Max)
{
    int diff = Max-Min;
    return (int) (((double)(diff+1)/RAND_MAX) * rand() + Min);
}

int main()
{
	time_t systime;
	time(&systime);
	srand((unsigned int)systime);

	int *g;
	int *new_g;
	int gsize;
	unsigned long int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;

	/*
	 * start with graph of size 8
	 */
	gsize = 8;
	g = (int *)malloc(gsize*gsize*sizeof(int));
	if(g == NULL) {
		exit(1);
	}

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
	memset(g,0,gsize*gsize*sizeof(int));

	while(gsize < 206)
	{
		/*
		 * find out how we are doing
		 */
		count = CliqueCount(g,gsize);
		printf("%d\n", count);

		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			//PrintGraph(g,gsize);

			/* Save counterexample into a file */
			SaveGraph(g,gsize);

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
		best_count = BIGCOUNT;
		double Tred = 0.9; // Reduction factor used for the cooling schedule of the temperature 
#define N_ITR 1E3
		int itr = 0;
		double T = 200;
		double Tmin = 0.008;
		do
		{
			while((itr < N_ITR) && best_count > 0)
			{
				/* Randomly pick an edge */
				int i = RandRange(0, gsize-1);
				int j = RandRange(i+1, gsize-1);
				//printf("i = %d, j = %d", i, j);
				//sleep(1);
//#ifdef EDGEONLY
//				if(FIFOFindEdge(taboo_list,i,j))
//#else
//				if(FIFOFindEdgeCount(taboo_list,i,j,best_count))
//#endif
//				{
//					 continue;
//				}

				/*
				 * flip it
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				unsigned long int curr_count = CliqueCount(g,gsize);
				double delta = curr_count - best_count; 

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];

				itr++;

				double rand = RandRange(0,1.0);
				if((delta <= 0 || rand < exp(-delta/(7*T)))) {
					/*
					 * flip it
					 */
					puts("test");
					g[i*gsize+j] = 1 - g[i*gsize+j];
					best_count = curr_count;
//#ifdef EDGEONLY
//					FIFOInsertEdge(taboo_list,i,j);
//#else
//					FIFOInsertEdgeCount(taboo_list,i,j,best_count);
//#endif
				}
			}
			T *= Tred;
		}while(best_count > 0 && T > Tmin);

	}

	FIFODeleteGraph(taboo_list);


	return(0);
}
