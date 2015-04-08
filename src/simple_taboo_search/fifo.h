#if !defined(FIFO_H)
#define FIFO_H


int FIFOInsertEdgeCount(void *in_f, int i, int j, int count);
int FIFOFindEdgeCount(void *in_f, int i, int j, int count);
void FIFODeleteEdgeCount(void *in_f);
void *FIFOInitEdgeCount(int size);
void *FIFOResetEdgeCount(void *in_f);

#define FIFOInsertEdge(f,i,j) FIFOInsertEdgeCount((f),(i),(j),0)
#define FIFOFindEdge(f,i,j) FIFOFindEdgeCount((f),(i),(j),0)
#define FIFODeleteEdge(f,i,j) FIFODeleteEdgeCount((f),(i),(j),0)
#define FIFOInitEdge(s) FIFOInitEdgeCount((s))
#define FIFOResetEdge(f) FIFOResetEdgeCount((f))

int FIFOInsertGraph(void *in_f, int *g, int gsize);
int FIFOFindGraph(void *in_f, int *g, int gsize);
void FIFODeleteGraph(void *in_f);
void *FIFOInitGraph(int size);
void *FIFOResetGraph(void *in_f);

int MakeGraphKey(int *s, int ssize, char **key);
int FIFOCount(void *in_f);


#define MAXKEYSIZE (256)


#endif

