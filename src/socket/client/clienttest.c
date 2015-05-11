#include <limits.h>

#include "msg.h"
#include "client.h"
#include "graph_utils.h"
#include "utils.h"
#include "taboo_search_client.h"

int main(int argc, char *argv[]) {
	char* matrix;
	char feedback[READBUFFERSIZE];
	int *g;
	int gsize = 0;
	int gccounter = INT_MAX;
	char* size;

	size = NumtoString(gsize);

	bzero(feedback, READBUFFERSIZE);
	if(sendRequest(HOSTNAME, SERVERPORT, size, feedback)) {
		printf("%s\n", feedback);
	}
	int newSize;
	int newCount;
	
	g = parseMessage(feedback, &newSize, &newCount);
	if (g != NULL) {
		tabooSearch(g, newSize);
	} else {
		printf("Deny!\n");
	}
	
	free(matrix);
	free(size);

	return 0;
}
