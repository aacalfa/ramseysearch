#include "client.h"
#include "graph_utils.h"
#include "utils.h"
#include "taboo_search_client.h"
int main(int argc, char *argv[]) {
	char* hostname;
	char* matrix;
	char feedback[READBUFFERSIZE];
	int *g;
	int gsize;
	int gccounter;
	char* size;
	char* counter;
	
	hostname = "localhost";

	ReadGraph("n109.txt", &g, &gsize);
	matrix = GraphtoChar(g,gsize);
	gccounter = 9;
	
	size = NumtoString(gsize);
	counter = NumtoString(gccounter);

	bzero(feedback, READBUFFERSIZE);
	if(sendResult(hostname, SERVERPORT, matrix, size, counter, feedback)) {
		printf("%s\n", feedback);
	}

	bzero(feedback, READBUFFERSIZE);
	if(sendRequest(hostname, SERVERPORT, size, feedback)) {
		printf("%s\n", feedback);
	}

	bzero(feedback, READBUFFERSIZE);
	if(sendResult(hostname, SERVERPORT, matrix, size, counter, feedback)) {
		printf("%s\n", feedback);
	}
	
	g = NULL;
	tabooSearch(g, -1);

	free(matrix);
	free(counter);
	free(size);

	return 0;
}
