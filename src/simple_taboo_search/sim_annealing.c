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

/* TODO: In order to apply the SA method to a specific problem, one must specify the following parameters: the state space, the energy (goal) function E(), the candidate generator procedure neighbour(), the acceptance probability function P(), and the annealing schedule temperature() AND initial temperature <init temp>.
*/

/* State structure
 */
typedef struct state {
	int noCliques;
	double colorRate;
} State;

/*
 * Goal: noCliques must be equal to zero
 *       and colorRate must be close to 0.5
 */
double Energy(State *s) {
}

/* Acceptance Probability Function
 *
 */
double AcceptProb(double energy, double new_energy, double temperature) {
	if( new_energy < energy )
		return 1.0;

	return exp(energy - new_energy)/temperature;
}

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

	/* Initialize state structure */
	State *engy = (State*) malloc(sizeof(State));
	engy->noCliques = INT_MAX;
	engy->colorRate = 1.0;

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
#define N_ITR 1E6
		int itr = 0;
		double T = 80;
		double Tmin = 0.00008;
		do
		{
			while((itr < N_ITR) && best_count > 0)
			{
				/* Randomly pick an edge */
				int i = RandRange(0, gsize-1);
				int j = RandRange(i+1, gsize-1);
				printf("i = %d, j = %d", i, j);
				sleep(1);
#ifdef EDGEONLY
				if(FIFOFindEdge(taboo_list,i,j))
#else
				if(FIFOFindEdgeCount(taboo_list,i,j,best_count))
#endif
				{
					 continue;
				}

				/*
				 * flip it
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				unsigned long int curr_count = CliqueCount(g,gsize);
				printf("curr_count: %lu\n", curr_count);
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
					g[i*gsize+j] = 1 - g[i*gsize+j];
					best_count = curr_count;
#ifdef EDGEONLY
					FIFOInsertEdge(taboo_list,i,j);
#else
					FIFOInsertEdgeCount(taboo_list,i,j,best_count);
#endif
				}
			}
			T *= Tred;
		}while(best_count > 0 || T > Tmin);

#if 0
		for(i=0; i < gsize; i++)
		{
			for(j=i+1; j < gsize; j++)
			{
				/*
				 * flip it
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				count = CliqueCount(g,gsize);

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

		/*
		 * rinse and repeat
		 */
#endif
	}

	FIFODeleteGraph(taboo_list);


	return(0);
}
