#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "clique_count.h"

int main() {
    int *g;
    int *ecounts;
    
    int gsize = 9;
	g = (int *)malloc(gsize*gsize*sizeof(int));
	if(g == NULL) {
		exit(1);
	}
    
    memset(g,0,gsize*gsize*sizeof(int));
    
    for(int j = 1; j < 9; j++) {
        //g[0*gsize+j] = 1;
        for(int i = 1; i < j; i++) {
            //g[i*gsize+j] = 1;
        }
    }

    ecounts = (int *)malloc(gsize*gsize*sizeof(int));
    CliqueCountAll(g,gsize,ecounts);
    
    for(int i = 0; i < gsize-1; i++) {
        for(int j = i+1; j < gsize; j++) {
            int edge_count = CliqueCountEdge(g, gsize, i, j);
            if(ecounts[i*gsize+j] != edge_count) {
                printf("ERROR: contradictory edge count:\n");
            }
            
            printf("Edge (%d, %d)\thas ecount\t%d\tedge count\t%d\n", i, j, ecounts[i*gsize+j], edge_count);
            
        }
    }
    
    printf("Total count: %d\n", CliqueCount(g, gsize));
}  