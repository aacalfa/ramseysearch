#ifndef CLIQUE_COUNT_H
#define CLIQUE_COUNT_H

int EdgeColor(int *g, int gsize, int i, int j);
int CliqueCount(int *g, int gsize);
int CliqueCountEdge(int *g, int gsize, int i, int j);
int CliqueCountAll(int *g, int gsize, int *ecounts);

unsigned long long CliqueCountULL(int *g, int gsize);
unsigned long long CliqueCountEdgeULL(int *g, int gsize, int i, int j);
unsigned long long CliqueCountAllULL(int *g, int gsize, unsigned long long *ecounts);
#endif

