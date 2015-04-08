#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "graph_utils.h"
#include "clique_count.h"

#define ARGS "f:"

char Fname[255];

int
main(int argc,char *argv[])
{
	char c;
	int *g;
	int gsize;
	int count;

	while((c = getopt(argc,argv,ARGS)) != EOF)
	{
		switch(c)
		{
			case 'f':
				strncpy(Fname,optarg,sizeof(Fname));
				break;
			default:
				fprintf(stderr,
				"test_clique_count  unrecognized argument: %c\n",c);
				fflush(stderr);
				break;
		}
	}

	if(!ReadGraph(Fname,&g,&gsize))
	{
		fprintf(stderr,"test_clique_count cant read graph\n");
		fflush(stderr);
		exit(1);
	}

	PrintGraph(g,gsize);
	
	count = CliqueCount(g,gsize);
	if(count == 0)
	{
		fprintf(stdout,
		"file %s contains a counter example of size %d\n",
			Fname,
			gsize);
	}
	else
	{
		fprintf(stdout,
	"file %s contains a graph of size %d with %d monochromatic cliques\n",
			Fname,
			gsize,
			count);
	}

	free(g);
	return(0);

}
