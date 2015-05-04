#ifndef CLIQUE_COUNT_H
#define CLIQUE_COUNT_H

unsigned long int CliqueCount(int *g, int gsize);
unsigned long int CliqueCountEdge(int *g, int gsize, int i, int j);
unsigned long int CliqueCountAll(int *g, int gsize, unsigned long int *ecounts);

#endif

