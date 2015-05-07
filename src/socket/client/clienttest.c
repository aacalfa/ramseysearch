#include "client.h"
#include "graph_utils.h"
#include "utils.h"
int main(int argc, char *argv[]) {
	char* hostname;
	char* matrix;
	char feedback[READBUFFERSIZE];
	int *g;
	int gsize;
	int gccounter;
//	char  size[3];
	char* size;
	char* counter;
	
	hostname = "localhost";
//	matrix = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";

	ReadGraph("n109.txt", &g, &gsize);
        matrix = GraphtoChar(g,gsize);
        gccounter = 9;
        
        size = NumtoString(gsize);
        counter = NumtoString(gccounter);
        
//	sprintf(size, "%d", gsize);
	//printf("%s",size);

	bzero(feedback, READBUFFERSIZE);
	if(sendResult(hostname, SERVERPORT, matrix, size, counter, feedback)) {
		printf("%s\n", feedback);
	}
	free(matrix);

	bzero(feedback, READBUFFERSIZE);
	if(sendRequest(hostname, SERVERPORT, feedback)) {
		printf("%s\n", feedback);
	}

	return 0;
}
