#include "client.h"
#include "graph_utils.h"
int main(int argc, char *argv[]) {
	char* hostname;
	char* matrix;
	char result[READBUFFERSIZE];
	int *g;
	int gsize;

	hostname = "localhost";
//	matrix = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";

	ReadGraph("n8.txt", &g, &gsize);
        matrix = GraphtoChar(g,gsize);

	if(sendCounterExample(hostname, SERVERPORT, matrix, result)) {
		printf("%s\n", result);
	}
	free(matrix);
	return 0;
}
