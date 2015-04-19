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
#include <ctype.h>


#include "jval.h"
#include "jrb.h"

#define TEST_SIZE (5)

#define SIZE (512)

extern char *optarg;

int SelfTest;
#define SERVER_ARGS "f:g:T"
char *Usage = "iso_check -f file1 -g file2\n\
\t-T run self test\n";

extern int IsIsomorph(int *g1, int *g2, int s);


FILE *Err_fd;
FILE *Log_fd;

struct configuration
{
        int size;
        int count;
        int type;
        int extra[25];
        int config[SIZE*SIZE];
};

struct configuration Config1;
struct configuration Config2;

char Gfile1[4096];
char Gfile2[4096];

void
PrintG(FILE *fg,
       int *g,
       int size)
{
        int i;
        int j;
        for(i=0; i < size; i++)
        {
                for(j=0; j < size; j++)
                        fprintf(fg,"%d ",g[i*size+j]);
                fprintf(fg,"\n");
        }
        fprintf(fg,"\n\n");
        fflush(fg);
	
	return;
}

int
ReadState(char *fname,
	  struct configuration *c)
{
        int i;
        int j;
	FILE *fd;
	int lsize;
	char line_buff[255];
	char *curr;
	char *err;
	int *g;
	char *tempc;
	int lcount;

	g = c->config;

	fd = fopen(fname,"r");
	if(fd == 0)
	{
		fprintf(Err_fd,"ReadState cannot open file %s\n",fname);
		fflush(Err_fd);
		return(0);
	}

	fgets(line_buff,254,fd);
	if(feof(fd))
	{
		fprintf(Err_fd,"ReadState eof on size\n");
		fflush(Err_fd);
		fclose(fd);
		return(0);
	}	
	if(!isdigit(line_buff[0]))
	{
		fprintf(Err_fd,"ReadState format error on size\n");
		fflush(Err_fd);
		fclose(fd);
		return(0);
	}
	tempc = line_buff;
	lsize = (int)strtol(tempc,&tempc,10);
	if(lsize > SIZE)
	{
		fprintf(Err_fd,"ReadState size too big, read: %d, max: %d\n",
			lsize,SIZE);
		fflush(Err_fd);
		fclose(fd);
		return(0);
	}

	lcount = (int)strtol(tempc,&tempc,10);

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
			sscanf(curr,"%d ",&(g[i*lsize+j]));
			curr += 2;
		}
	}

	if(i < lsize)
	{
		fprintf(Err_fd,"ReadState file too short, lsize: %d\n",lsize);
		fflush(Err_fd);
		fclose(fd);
		return(0);
	}

	c->size = lsize;
	c->count = lcount;
				
	fclose(fd);
/*
	fprintf(Log_fd,"Memory initializing with:\n");
	fprintf(Log_fd,"%d\n",lsize);
	PrintG(Log_fd,g,lsize);
*/
	return(1);
}

/*
 * for self-test
 */
void PermuteVertices(int *g, int size, int v1, int v2)
{
	JRB v1_a;
	JRB v2_b;
	JRB rb;
	int i;
	int j;

	v1_a = make_jrb();
	v2_b = make_jrb();

	for(i=0; i < size; i++) {
		for(j=i+1; j < size; j++) {
			if(i == v1) {
				rb = jrb_find_int(v1_a,j);
				if(rb == NULL) {
					jrb_insert_int(v1_a,j,(Jval)g[i*size+j]);
				}
			}
			if(j == v1) {
				rb = jrb_find_int(v1_a,i);
				if(rb == NULL) {
					jrb_insert_int(v1_a,i,(Jval)g[i*size+j]);
				}
			}
			if(i == v2) {
				rb = jrb_find_int(v2_b,j);
				if(rb == NULL) {
					jrb_insert_int(v2_b,j,(Jval)g[i*size+j]);
				}
			}
			if(j == v2) {
				rb = jrb_find_int(v2_b,i);
				if(rb == NULL) {
					jrb_insert_int(v2_b,i,(Jval)g[i*size+j]);
				}
			}
		}
	}

	jrb_traverse(rb,v1_a) {
		j = rb->key.i;
		/*
		 * leave edges between swapped nodes alone
		 */
		if(j > v2) {
			g[v2*size+j] = rb->val.i;
		} else if(j < v2) {
			g[j*size+v2] = rb->val.i;
		}
	}
	jrb_traverse(rb,v2_b) {
		j = rb->key.i;
		if(j > v1) {
			g[v1*size+j] = rb->val.i;
		} else if(j < v1) {
			g[j*size+v1] = rb->val.i;
		}
	}

	jrb_free_tree(v1_a);
	jrb_free_tree(v2_b);


	return;
}

int
main(int argc,char *argv[])
{
	char c;
	int count;
	int iso;
	int i;
	int j;
	int k;
	int test_size;


	Log_fd = stdout;
	Err_fd = stderr;

	SelfTest = 0;
	while((c = getopt(argc,argv,SERVER_ARGS)) != EOF)
	{
		switch(c)
		{
			case 'f':
				strncpy(Gfile1,optarg,sizeof(Gfile1));
				break;
			case 'g':
				strncpy(Gfile2,optarg,sizeof(Gfile2));
				break;
			case 'T':
				SelfTest = 1;
				break;
			default:
				fprintf(Err_fd,
				"Memory Server unrecognized argument: %c\n",c);
				fflush(stderr);
				fprintf(stderr,"%s",Usage);
				break;
		}
	}

	if(!ReadState(Gfile1,&Config1))
	{
		fprintf(Err_fd,"Memory cant read state from %s\n",
			Gfile1);
		fflush(Err_fd);
		exit(1);
	}
	if(!ReadState(Gfile2,&Config2))
	{
		fprintf(Err_fd,"Memory cant read state from %s\n",
			Gfile2);
		fflush(Err_fd);
		exit(1);
	}

	if(Config1.size != Config2.size) {
		printf("NO: not isomorphs -- different sizes: f: %d g: %d\n",
			Config1.size,
			Config2.size);
		exit(0);
	}

	if(SelfTest == 1) {
		printf("permutations\n");
		test_size = Config2.size;
		memcpy(Config1.config,Config2.config,sizeof(Config1.config));
		for(k=0; k < 1000; k++) {
			i = (int)(drand48()*test_size);
			j = (int)(drand48()*test_size);
			while(j == i) {
				j = (int)(drand48()*test_size);
			}
			PermuteVertices(Config2.config,test_size,i,j);
			printf("i: %d, j: %d ",i,j);
			iso = IsIsomorph(Config1.config,Config2.config,
					test_size);
			if(iso == 1) {
				printf("YES\n");
			} else {
				printf("NO\n");
				exit(1);
			}
		}

		/*
		 * now test non-isomorph
		 */
		printf("randoms\n");
		for(k=0; k < 1000; k++) {
			i = (int)(drand48()*test_size);
			j = (int)(drand48()*test_size);
			while(j == i) {
				j = (int)(drand48()*test_size);
			}
			/*
			 * flip a bit at random
			 */
			Config2.config[i*test_size+j] = 1 - 
				Config2.config[i*test_size+j];
			printf("i: %d, j: %d ",i,j);
			iso = IsIsomorph(Config1.config,Config2.config,
					test_size);
			if(iso == 1) {
				printf("YES\n");
				exit(1);
			} else {
				printf("NO\n");
			}
		}
		exit(0);
	}
		

	iso = IsIsomorph(Config1.config,Config2.config,Config1.size);
	if(iso == 1) {
		printf("YES: isomorphs\n");
	} else {
		printf("NO: not isomorphs\n");
	}

	exit(0);
	
}
