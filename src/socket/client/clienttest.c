#include "msg.h"
#include "client.h"
#include "graph_utils.h"
#include "utils.h"
#include "taboo_search_client.h"

int main(int argc, char *argv[]) {
	char* matrix;
	char feedback[READBUFFERSIZE];
	int *g;
	int gsize;
	int gccounter;
	char* size;
	char* counter;

	size = NumtoString(gsize);
	counter = NumtoString(gccounter);

	bzero(feedback, READBUFFERSIZE);
	if(sendRequest(HOSTNAME, SERVERPORT, size, feedback)) {
		printf("%s\n", feedback);
	}
	int* newSize;
	int* newCount;
	
	g = parseMessage(feedback, newSize,newCount);
	if (g != NULL) {
		tabooSearch(g, *newSize);
	} else {
		printf("Deny!\n");
	}
	
	// TODO: CLIENT NEEDS TO KNOW HOW TO PARSE THE MESSAGE SENT FROM THE SERVER AFTER CALLING
	// SENDREQUEST
	//g = ;
	

	free(matrix);
	free(counter);
	free(size);

	return 0;
}
