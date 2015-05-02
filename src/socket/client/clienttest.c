#include "client.h"
#include "graph_utils.h"
#include "format.h"
int main(int argc, char *argv[]) {
	char* hostname;
	char* matrix;
	char result[READBUFFERSIZE];
	int *g;
	int gsize;
//	char  size[3];
	char* size;

	hostname = "localhost";
//	matrix = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";

	ReadGraph("n109.txt", &g, &gsize);
        matrix = GraphtoChar(g,gsize);
        size = tostring(gsize);
//	sprintf(size, "%d", gsize);
	printf("%s",size);
	if(sendCounterExample(hostname, SERVERPORT, matrix, size, result)) {
		printf("%s\n", result);
	}
	free(matrix);
	return 0;
}
