#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#define MAXSIZE (541)

int* ChartoGraph(char *g, int gsize)
{
	int *graph = (int*) calloc(gsize*gsize,sizeof(int));

	int i, j;
	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			graph[i*gsize+j] = g[i*gsize+j] - '0';
		}
	}
	return graph;
}
/*
 * Converts a graph matrix from int to char
 */
char* GraphtoChar(int *g, int gsize)
{
	int charsize = (gsize*gsize) + 1;
	char *graph = (char*) calloc(charsize,sizeof(char));

	int i, j;
	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			graph[i*gsize+j] = g[i*gsize+j] + '0';
		}
	}
	graph[gsize*gsize] = '\0';
	return graph;
}

/*
 * Calculates how many green and red edges in the
 * given graph
 */
void GetNumEdgeColors(int *g, int gsize, int *red, int*green)
{
	int i, j;
	for(i=0; i < gsize - 1; i++)
	{
		for(j=i+1; j < gsize; j++)
		{
			int color = g[i*gsize+j];
			if(color == 0)
				(*red)++;
			else
				(*green)++;
		}
	}
}

/*
 * CopyGraph 
 *
 * copys the contents of old_g to corresponding locations in new_g
 * leaving other locations in new_g alone
 * that is
 * new_g[i,j] = old_g[i,j]
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

 /*
 * prints in the right format for the read routine
 */
void SaveGraph(int *g, int gsize, char *dir_name)
{
	int i;
	int j;

	char filename[BUFSIZ];
	sprintf(filename, "%s/n%d.txt", dir_name, gsize);

	/* Check if a file with the same name already exists */
	if( access( filename, F_OK ) != -1 ) {
		/* file exists */
		/* Rename it so that there is no conflict */
		char old_graphname[BUFSIZ];
		char *new_graphname = NULL;

		sprintf(old_graphname, "n%d.txt", gsize);

		/* Add timestamp to differentiate */
		asprintf(&new_graphname, "n%d_%u.txt", gsize, (unsigned)time(NULL));

		char * pch;
		/* Find occurence of old_graphname in filename */
		pch = strstr(filename, old_graphname);
		/* Replace it with new_graphname */
		strncpy (pch, new_graphname, strlen(new_graphname));
	}

	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f,"%d\n",gsize);

	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(f,"%d ",g[i*gsize+j]);
		}
		fprintf(f,"\n");
	}

	fclose(f);
	return;
}

/*
 * prints in the right format for the read routine
 */
void PrintGraphChar(char *g, int gsize)
{
	int i;
	int j;

	fprintf(stdout,"%d\n",gsize);

	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(stdout,"%c ",g[i*gsize+j]);
		}
		fprintf(stdout,"\n");
	}

	return;
}

/*
 * prints in the right format for the read routine
 */
void PrintGraph(int *g, int gsize)
{
	int i;
	int j;

	fprintf(stdout,"%d\n",gsize);

	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(stdout,"%d ",g[i*gsize+j]);
		}
		fprintf(stdout,"\n");
	}

	return;
}

/*
 * reads a graph of the correct format from the file referenced
 * by #fname#.  The resulting is passed back through the #g# and #gsize#
 * out parameters.  The space for the file is malloced and the routine
 * is not thread safe.  Returns 1 on success and 0 on failure.
 */
int
ReadGraph(char *fname,
	  int **g,
	  int *gsize)
{
        int i;
        int j;
	FILE *fd;
	int lsize;
	int *lg;
	char line_buff[255];
	char *curr;
	char *err;
	char *tempc;
	int lcount;

	fd = fopen(fname,"r");
	if(fd == 0)
	{
		fprintf(stderr,"ReadGraph cannot open file %s\n",fname);
		fflush(stderr);
		return(0);
	}

	fgets(line_buff,254,fd);
	if(feof(fd))
	{
		fprintf(stderr,"ReadGraph eof on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}	
	i = 0;
	while((i < 254) && !isdigit(line_buff[i]))
		i++;
	
	/*
	 * first line of the file must contain a size
 	 */
	if(!isdigit(line_buff[i]))
	{
		fprintf(stderr,"ReadGraph format error on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}
	tempc = line_buff;
	lsize = (int)strtol(tempc,&tempc,10);
	if((lsize < 0) || (lsize > MAXSIZE))
	{
		fprintf(stderr,"ReadGraph size bad, read: %d, max: %d\n",
			lsize,MAXSIZE);
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	lg = (int *)malloc(lsize*lsize*sizeof(int));
	if(lg == NULL)
	{
		fprintf(stderr,"ReadGraph: no space\n");
		fflush(stderr);
		return(0);
	}

	memset(lg,0,lsize*lsize*sizeof(int));

        for(i=0; i < lsize; i++)
        {
		if(feof(fd))
		{
			break;
		}
		err = fgets(line_buff,254,fd);
		if(err == NULL)
		{
			break;
		}
		curr = line_buff;
                for(j=0; j < lsize; j++)
		{
			sscanf(curr,"%d ",&(lg[i*lsize+j]));
			if((lg[i*lsize+j] != 1) && 
			   (lg[i*lsize+j] != 0))
			{
				fprintf(stderr,
			"ReadGraph: non-boolean value read: %d\n", 
					lg[i*lsize+j]);
				fflush(stderr);
				fclose(fd);
				return(0);
			}
			while(isdigit(*curr))
				curr++;
			while(isspace(*curr))
				curr++;
		}
	}

	if(i < lsize)
	{
		fprintf(stderr,"ReadGraph file too short, lsize: %d\n",lsize);
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	fclose(fd);

	*g = lg;
	*gsize = lsize;
	return(1);
}
	
