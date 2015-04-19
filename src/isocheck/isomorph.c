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

#include "jval.h"
#include "jrb.h"

/*
 * undefine FAST and the slower, more thorough version will be enabled
 */
#define FAST

/*
 * count the number of edges of a specified color incident on node v
 */
int ColorCount(int *g, int s, int v, int color)
{
	int k = 0;
	int i;

	/*
	 * g is assumed to upper triangular with colors in off diagional
	 */
	for(i=v+1; i < s; i++) {
		if(g[v*s+i] == color) {
			k++;
		}
	}
	for(i=0; i < v; i++) {
		if(g[i*s+v] == color) {
			k++;
		}
	}

	return(k);
}

/*
 * this is the faster version -- it only goes 2 deep
 */
JRB MakeVertexList2(int *g, int s, int color)
{
	JRB list;
	JRB minor;
	int i;
	int j;
	int count;

	/*
	 * list of top level nodes
	 */
	list = make_jrb();
	if(list == NULL) {
		exit(1);
	}

	for(i=0; i < s; i++) {
		/*
		 * list of nodes attached to each top level node
		 */
		minor = make_jrb();
		if(minor == NULL) {
			exit(1);
		}
		for(j=0; j < s; j++) {
			/*
			 * no edges to nodes themselves
			 */
			if(j == i) {
				continue;
			}
			/*
			 * only look at subgraphs of a single color
			 *
			 * restrict to distinguished color
			 */
			if((i < j) && (g[i*s+j] != color)) {
				continue;
			} else if((i > j) && (g[j*s+i] != color)) {
				continue;
			}
			count = ColorCount(g,s,j,color);
			/*
			 * sort minor list by color count
			 */
			jrb_insert_int(minor,count,(Jval)NULL);
		}
		count = ColorCount(g,s,i,color);
		/*
		 * sort major list by color count
		 */
		jrb_insert_int(list,count,(Jval)(void *)minor);
	}

	return(list);
}

/*
 * slower version -- goes three deep
 */
JRB MakeVertexList3(int *g, int s, int color)
{
	JRB list;
	JRB minor;
	JRB minor2;
	int i;
	int j;
	int k;
	int count;

	/*
	 * top level list of nodes
	 */
	list = make_jrb();
	if(list == NULL) {
		exit(1);
	}

	for(i=0; i < s; i++) {
		/*
		 * second level list of nodes for each top level node
		 */
		minor = make_jrb();
		if(minor == NULL) {
			exit(1);
		}
		for(j=0; j < s; j++) {
			if(j == i) {
				continue;
			}
			/*
			 * make third level list
			 */
			minor2 = make_jrb();
			if(minor2 == NULL) {
				exit(1);
			}
			for(k=0; k < s; k++) {
				if(k == j) {
					continue;
				}
				if(k == i) {
					continue;
				}
			/*
			 * consider subgraphs of only one color
			 *
			 * restrict to distinguished color
			 */
				if((j < k) && (g[j*s+k] != color)) {
					continue;
				} else if((j > k) && (g[k*s+j] != color)) {
					continue;
				}
				count = ColorCount(g,s,k,color);
				/*
				 * sort third level list by color count
				 */
				jrb_insert_int(minor2,count,(Jval)NULL);
			}
			count = ColorCount(g,s,j,color);
			/*
			 * sort second level list by color count
			 */
			jrb_insert_int(minor,count,(Jval)(void *)minor2);
		}
		count = ColorCount(g,s,i,color);
		/*
		 * sort top level list by color count
		 */
		jrb_insert_int(list,count,(Jval)(void *)minor);
	}

	return(list);
}

/*
 * works with either slow or fast versions
 */
void FreeVertexList(JRB list)
{
	JRB minor;
	JRB minor2;
	JRB rb;
	JRB mb;

	rb = jrb_first(list);
	while(rb != jrb_nil(list)) {
		minor = (JRB)rb->val.v;
		if(minor != NULL) {
			mb = jrb_first(minor);
			while(mb != jrb_nil(minor)) {
				minor2 = (JRB)mb->val.v;
				if(minor2 != NULL) {
					jrb_free_tree(minor2);
				}
				mb = jrb_next(mb);
			}
			jrb_free_tree(minor);
		}
		rb = jrb_next(rb);
	}
	jrb_free_tree(list);
	return;
}


/*
 * fast version -- compares two level structure
 */
int VertexListEq2(JRB l1, JRB l2)
{
	int count1;
	int count2;
	int count3;
	int okay;
	JRB j1;
	JRB j2;
	JRB j3;
	JRB m1;
	JRB m2;
	JRB m3;
	JRB p1;
	JRB p2;
	JRB p3;

	
	j1 = jrb_first(l1);

	/*
	 * walk the outter list of g1
	 */
	while(j1 != jrb_nil(l1)) {
		/*
		 * pull the color count
		 */
		count1 = j1->key.i;
		/* look for this count in the other list */
		j2 = jrb_find_int(l2,count1);
		/* if not there, no isomorph */
		if(j2 == NULL) {
			return(0);
		}
		/* find the first instance of this count */
		while((jrb_prev(j2) != jrb_nil(l2)) && 
				(jrb_prev(j2)->key.i == count1)) {
			j2 = jrb_prev(j2);
		}
		okay = 0;
		while((j2 != jrb_nil(l2)) && (j2->key.i == count1)) {

			p1 = (JRB)(j1->val.v);
			p2 = (JRB)(j2->val.v);
			m1 = jrb_first(p1);
			m2 = jrb_first(p2);
			okay = 1;
			/*
			 * walk the secondary list -- must match entry for
			 * entry
			 */
			while((m1 != jrb_nil(p1)) && (m2 != jrb_nil(p2))) {
				if(m1->key.i != m2->key.i) {
					okay = 0;
					break;
				}
				m1 = jrb_next(m1);
				m2 = jrb_next(m2);
			}
			/*
			 * test to see if one is longer than the other
			 * (paranoid)
			 */
			if((m1 != jrb_nil(p1)) && (m2 == jrb_nil(p2))) {
				return(0);
			}
			if((m1 == jrb_nil(p1)) && (m2 != jrb_nil(p2))) {
				return(0);
			}
			/*
			 * if they match, okay == 1 -> bail out and remove
			 * from both lists
			 */
			if(okay == 1) {
				break;
			}
			/*
			 * no match for this list in g2, try another
			 */
			j2 = jrb_next(j2);
		}

		/*
		 * if we don't find a match, then not iso
		 */
		if(okay == 0) {
			return(0);
		}

		/*
		 * here, if we found a match, delete from g1 and g2
		 */
		p1 = (JRB)(j1->val.v);
		p2 = (JRB)(j2->val.v);
		jrb_free_tree(p1);
		jrb_free_tree(p2);
		j1 = jrb_next(j1);
		jrb_delete_node(jrb_prev(j1));
		j2 = jrb_next(j2);
		jrb_delete_node(jrb_prev(j2));
		j1 = jrb_first(l1);
	}

	/*
	 * g1 is empty here,
	 * if there are still things left on the g2 list, then
	 * not a match
	 */
	if(jrb_first(l2) != jrb_nil(l2)) {
		return(0);
	}
			
	return(1);
}

/*
 * walk l2 looking for count1 at first level, count2 at second level
 * and a list matching l13
 *
 * delete list in l2 that matches l13 to prevent future duplicate matches
 */
int VertexListFindDelete(JRB l2,int count1,int count2,JRB l13)
{
	JRB rb21;
	JRB l22;
	JRB rb22;
	JRB l23;
	JRB sl1;
	JRB sl2;
	int match;

	rb21 = jrb_first(l2);
	/*
	 * walk first level of l2
	 */
	while(rb21 != jrb_nil(l2)) {
		/*
		 * each entry at level 1 is a list and a count
		 */
		l22 = (JRB)rb21->val.v;
		/*
		 * skip it if it has been deleted (paranoid)
		 */
		if(l22 == NULL) {
			rb21 = jrb_next(rb21);
			continue;
		}
		/*
		 * if the count at level 1 does not match, skip it
		 */
		if(rb21->key.i < count1) {
			rb21 = jrb_next(rb21);
			continue;
		}
		/*
		 * bail out if we have gone too far
		 */
		if(rb21->key.i > count1) {
			break;
		}

		/*
		 * otherwise, walk this list at level 2 looking for a count2
		 * match
		 */
		rb22 = jrb_first(l22);
		while(rb22 != jrb_nil(l22)) {
			/*
			 * of the count doesn't match, skip it
			 */
			if(rb22->key.i < count2) {
				rb22 = jrb_next(rb22);
				continue;
			}
			if(rb22->key.i > count2) {
				break;
			}

			l23 = (JRB)rb22->val.v;
			/*
			 * if the list has been deleted, skip it
			 */
			if(l23 == NULL) {
				rb22 = jrb_next(rb22);
				continue;
			}
			/*
			 * here the count at level 1 and level 2 matches
			 * and we have a level 3 list.  compare level 3 list
			 * with level 3 list passed in as an arg
			 */
			sl1 = jrb_first(l13);
			sl2 = jrb_first(l23);
			match = 1;
			while((sl1 != jrb_nil(l13)) &&
			      (sl2 != jrb_nil(l23))) {
				/*
				 * we miss, skip skip
				 */
				if(sl1->key.i != sl2->key.i) {
					match = 0;
					break;
				}
				sl1 = jrb_next(sl1);
				sl2 = jrb_next(sl2);
			}
			if(match == 1) {
				/*
				 * if match, and we are at the end of both
				 * lists, delete and return
				 */
				if((sl1 == jrb_nil(l13)) &&
				   (sl2 == jrb_nil(l23))) {
					jrb_free_tree(l23);
					rb22->val.v = NULL;
					return(1);
				}
			}
			rb22 = jrb_next(rb22);
		}
		rb21 = jrb_next(rb21);
	}
	/*
	 * here we couldn't find a match in l2 anywhere -- just
	 * return
	 */
	return(0);
}

int VertexListEq3(JRB l1, JRB l2)
{
	JRB rb11;
	JRB l12;
	JRB rb12;
	JRB l13;
	int count1;
	int count2;
	int found;

	rb11 = jrb_first(l1);
	while(rb11 != jrb_nil(l1)) {
		count1 = rb11->key.i;
		l12 = (JRB)rb11->val.v;
		rb12 = jrb_first(l12);
		while(rb12 != jrb_nil(l12)) {
			count2 = rb12->key.i;
			l13 = (JRB)rb12->val.v;
			found = VertexListFindDelete(l2,count1,count2,l13);
			if(found == 0) {
				return(0);
			}
			rb12 = jrb_next(rb12);
		}
		rb11 = jrb_next(rb11);
	}
	return(1);
}

/*
 * red is zero, blue is one
 * best color is on with fewer edges
 */
int BestColor(int *g, int gsize)
{
	int i;
	int j;
	int reds;
	int blues;
	int color;

	reds=0;
	blues=0;

	for(i=0; i < gsize; i++) {
		for(j=i+1; j < gsize; j++) {
			if(g[i*gsize+j] == 0) {
				reds++;
			} else {
				blues++;
			}
		}
	}

	if(reds > blues) {
		color = 1;
	} else {
		color = 0;
	}
	return(color);
}

int IsIsomorph(int *g1, int *g2, int s1)
{
	int i;
	int j;
	int max1;
	int max1_i;
	int max2;
	int max2_i;
	int max3;
	int max3_i;
	int k;
	int color;
	int color2;
	JRB l1;
	JRB l2;
	int eq;

	/*
	 * check for complements
	 *
	 * complement g2 if best colors are not the same to be sure
	 * matching right pair of complements
	 */
	color = BestColor(g1,s1);
	if(color < 0) {
		return(0);
	}

	color2 = BestColor(g2,s1);
	if(color2 < 0) {
		return(0);
	}

	if(color != color2) {
		for(i=0; i < s1; i++) {
			for(j=i+1; j < s1; j++) {
				g2[i*s1+j] = 1 - g2[i*s1+j];
			}
		}
	}


	

	/*
	 * start by finding the maximum number of blue edges 
	 * eminating from any vertex in g1
	 */
#ifdef FAST
	l1 = MakeVertexList2(g1,s1,color);
#else
	l1 = MakeVertexList3(g1,s1,color);
#endif
	if(l1 == NULL) {
		return(0);
	}

#ifdef FAST
	l2 = MakeVertexList2(g2,s1,color);
#else
	l2 = MakeVertexList3(g2,s1,color);
#endif
	if(l2 == NULL) {
		FreeVertexList(l1);
		return(0);
	}

#ifdef FAST
	eq = VertexListEq2(l1,l2);
#else
	eq = VertexListEq3(l1,l2);
#endif
	FreeVertexList(l1);
	FreeVertexList(l2);
	return(eq);
}

