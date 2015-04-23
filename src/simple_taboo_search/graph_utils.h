#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

void PrintGraph(int *g, int gsize);
int ReadGraph(char *fname, int **g, int *gsize);
void SaveGraph(int *g, int gsize);
void GetNumEdgeColors(int *g, int gsize, int *red, int*green); 
void CopyGraph(int *old_g, int o_gsize, int *new_g, int n_gsize);

#endif

