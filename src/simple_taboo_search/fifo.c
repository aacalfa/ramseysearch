#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "jval.h"
#include "jrb.h"
#include "dllist.h"

#include "fifo.h"

/**
 ** builds a doubly-linked list thhat is indexed by a red-black
 ** tree.  The linked-list and red-black tree codes are due to
 ** Dr. James Plank
 **/


struct fifo_stc
{
	JRB findex;
	Dllist fifo;
	int fcount;
	int max_size;
};

typedef struct fifo_stc FIFO_t;


void *FIFOInitGraph(int size)
{
	FIFO_t *f;

	f = (FIFO_t *)malloc(sizeof(FIFO_t));
	if(f == NULL)
	{
		fprintf(stderr,"no space for FIFO\n");
		fflush(stderr);
		exit(1);
	}
	f->findex = make_jrb();
	f->fifo = new_dllist();
	f->fcount = 0;
	f->max_size = size;


	return((void *)f);
}


int FIFOCount(void *in_f)
{
	FIFO_t *f = (void *)in_f;

	return(f->fcount);
}


void
FIFODelete(void *in_f)
{
        FIFO_t *f = (void *)in_f;
        jrb_free_tree(f->findex);
        free_dllist(f->fifo);
        free(f);

        return;
}


/*
 * resets the fifo by deallocating and reallocating one of the same
 * size
 */
void *FIFOResetGraph(void *in_f)
{
	int size;
	FIFO_t *f = (void *)in_f;

	size = f->max_size;
	FIFODelete(f);

	f = FIFOInitGraph(size);

	return((void *)f);
}



void
FIFODeleteGraph(void *in_f)
{
	FIFO_t *f = (void *)in_f;
	Dllist dl;
	JRB jnode;
	char *key;

	dll_traverse(dl,f->fifo)
	{
		key = (char *)jval_v(dll_val(dl));
		jnode = jrb_find_str(f->findex,key);
		if(jnode != NULL)
		{
			jrb_delete_node(jnode);
		}
		else
		{
			fprintf(stderr,"error: delete graph no key %s\n",
					key);
			fflush(stderr);
		}
		/*
		 * MakeGraphKey does a malloc for the space necessary to
		 * hold the string
		 *
		 * need to free it here
		 */
		free(key);
	}

	jrb_free_tree(f->findex);
	free_dllist(f->fifo);
	f->fcount = 0;
	free(f);

	return;
}


#define MASKSIZE (128)
static unsigned char Mask[MASKSIZE];	/* should be big enough to 
					   hold all bit masks */

/*
 * MakeGraphKey:
 *
 * Turns a one-dimenstional array of size #gsize# containing the values 0 or 1 in
 * each element into a bit mask, and then converts the bit mask into an ascii
 * string through the outpointer #key#.  The space for the string is malloced
 * in the function so the caller is responsible for deallocating the
 * string's memory.   The length of the string produced is returned.  The
 * temporary space needed for the bit mask is a static global variable which
 * means that this function is not thread safe.
 */
int MakeGraphKey(int *s, int gsize, char **key)
{
	char *key_string;
	int key_string_size;
	int mask_byte_count;
	int index;
	int k;
	int i;
	int bit_count;
	int lsize;


	/*
	 * sanity check
	 */
	mask_byte_count = (gsize / 8) + 1;

	if(mask_byte_count > MASKSIZE)
	{
		fprintf(stderr,"MakeGraphKey: gsize to big: %d\n",
				gsize);
		fflush(stderr);
		exit(1);
	}
	if(key == NULL)
	{
		return(0);
	}

	/*
	 * clear the mask -- not that the choice of a static here
	 * saves malloc and free calls which, in the end, saves more time
	 * than multithreading would buy (probably)
	 */
	memset(Mask,0,MASKSIZE);

	/*
	 * walk through #s# one element at a time
	 */
	index = 0;
	bit_count = 0;
	/*
	 * zero'th entry not interesting size it is on the diagonal
	 */
	for(i=1; i < (gsize+1); i++)
	{
		/*
		 * if the current value is a one or in a 1
		 * otherwise it is a 0
		 */
		if(s[i] == 1)
		{
			Mask[index] |= 1;
		}
		bit_count++;
		/*
		 * if we have 8 bits in this byte of the mask,
		 * skip to the next byte
		 */
		if(bit_count == 8)
		{
			bit_count = 0;
			index++;
		}
		else
		{
			/*
		 	 * shift it over
		 	 */
			Mask[index] = Mask[index] << 1;
		}
	}


	/*
	 * here, we know how big a string we need
	 */
	mask_byte_count = index+1;
	key_string_size = (mask_byte_count * 2)+1;

	key_string = (char *)malloc(key_string_size * sizeof(char));
	lsize = key_string_size * sizeof(char);
	if(key_string == NULL)
	{
		fprintf(stderr,"MakeGraphKey: no space for key_string\n");
		fflush(stderr);
		exit(1);
	}
	memset(key_string,0,key_string_size);


	index = 0;
	for(k = 0; k < mask_byte_count; k++)
	{
		sprintf(&(key_string[index]),"%02x",(unsigned char)Mask[k]);
		index += 2;
	}


	*key = key_string;
	return(lsize);
	
}


/*
 * tabus the first row of the graph #g# having size #gsize#
 */
int
FIFOInsertGraph(void *in_f, int *g, int gsize)
{
	FIFO_t *f = (void *)in_f;
	JRB jnode;
	char *key;
	char *dkey;

	(void)MakeGraphKey(g,gsize,&key);

	/*
	 * okay -- this saves calls to mallocs
	 *
	 * the jval in the Dllist points ot a JRB node.  The key in
	 * the JRB node holds the value
	 */

	/*
	 * first, test to see if the fifo needs to discard an entry
	 * before one can be added.  add at the tail and discard at the
	 * head
	 */
	if((f->fcount+1) > f->max_size)
	{
		
		dkey = (char *)jval_v(dll_val(dll_first(f->fifo)));
		jnode = jrb_find_str(f->findex,dkey);

		if(jnode == NULL)
		{
			fprintf(stderr,
		"FIFOInsertGraph error: key %s not in index\n",
					dkey);
			fflush(stderr);
		}
		else
		{
			jrb_delete_node(jnode);
		}

		/*
		 * key is malloced in MakeGraphKey
		 */
		free(dkey);
		/*
		 * now, throw away head of Fifo list
		 */
		dll_delete_node(dll_first(f->fifo));
		f->fcount--;
	}


	/*
	 * append this key to the end of the dllist
	 */
	if(key == NULL)
	{
		fprintf(stderr,"FIFOInsertGraph error, inserted NULL\n");
		fflush(stderr);
	}
	else
	{
		dll_append(f->fifo,new_jval_v(key));

	/*
	 * stick the new key in the index
	 */
		jnode = jrb_insert_str(f->findex,key,new_jval_v(NULL));

	}


	f->fcount++;

	return(1);
}

/*
 * makes a key for the specified graph in #g# and searches for it in the
 * rb tree.  Returns 1 on find and 0 on no-find.
 */
int FIFOFindGraph(void *in_f, int *g, int gsize)
{
	FIFO_t *f = (void *)in_f;
	JRB jnode;
	char *key;
	int lsize;

	(void)MakeGraphKey(g,gsize,&key);

	jnode = jrb_find_str(f->findex,key);



	if(jnode == NULL)
	{
		free(key);
		return(0);
	}
	else
	{
		free(key);
		return(1);
	}
}

/*
 * these functions do the same thing as the ones shown above, but the
 * key is an append of (i,j)+count * and not and entire graph
 */
void *FIFOInitEdgeCount(int size)
{
	return(FIFOInitGraph(size));
}

void
FIFODeleteEdgeCount(void *in_f)
{
	FIFO_t *f = (void *)in_f;
	Dllist dl;
	JRB jnode;
	int key;

	dll_traverse(dl,f->fifo)
	{
		key = jval_i(dll_val(dl));
		jnode = jrb_find_int(f->findex,key);
		if(jnode != NULL)
		{
			jrb_delete_node(jnode);
		}
		else
		{
			fprintf(stderr,"error: delete graph no key %d\n",
					key);
			fflush(stderr);
		}
	}

	jrb_free_tree(f->findex);
	free_dllist(f->fifo);
	f->fcount = 0;
	free(f);

	return;
}

void *FIFOResetEdgeCount(void *in_f)
{
	int size;
	FIFO_t *f = (void *)in_f;

	size = f->max_size;
	FIFODelete(f);

	f = FIFOInitEdgeCount(size);

	return((void *)f);
}

double MakeEdgeCountKey(int distance, int count)
{
	double key;

	/*
	 * pack them both into a double
	 *
	 * won't work if the count exceeds 100000000
	 */
	key = (double)distance * 100000000.0 + (double)count;

	return(key);
}

int
FIFOInsertEdgeCount(void *in_f, int i, int j, int count)
{
	FIFO_t *f = (void *)in_f;
	JRB jnode;
	double dkey;
	double key;
	int distance;

	distance = (i << 16) | j;

	key = MakeEdgeCountKey(distance,count);


	/*
	 * okay -- this saves calls to mallocs
	 *
	 * the jval in the Dllist points ot a JRB node.  The key in
	 * the JRB node holds the value
	 */

	/*
	 * throw away at the head
	 */
	if((f->fcount+1) > f->max_size)
	{
		
		dkey = jval_d(dll_val(dll_first(f->fifo)));
		jnode = jrb_find_dbl(f->findex,dkey);

		if(jnode == NULL)
		{
			fprintf(stderr,
		"FIFOInsertEdgeCount error: key %f not in index\n",
					dkey);
			fflush(stderr);
		}
		else
		{
			jrb_delete_node(jnode);
		}

		/*
		 * now, throw away head of Fifo list
		 */
		dll_delete_node(dll_first(f->fifo));
		f->fcount--;
	}


	dll_append(f->fifo,new_jval_d(key));

	/*
 	 * stick the new key in the index
 	 */
	jnode = jrb_insert_dbl(f->findex,key,new_jval_v(NULL));


	f->fcount++;

	return(1);
}

int FIFOFindEdgeCount(void *in_f, int i, int j, int count) 
{
	FIFO_t *f = (void *)in_f;
	JRB jnode;
	double key;
	int distance;

	distance = (i << 16) | j;

	key = MakeEdgeCountKey(distance,count);

	jnode = jrb_find_dbl(f->findex,key);



	if(jnode == NULL)
	{
		return(0);
	}
	else
	{
		return(1);
	}
}
