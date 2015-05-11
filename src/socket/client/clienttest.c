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
	
	
	// TODO: CLIENT NEEDS TO KNOW HOW TO PARSE THE MESSAGE SENT FROM THE SERVER AFTER CALLING
	// SENDREQUEST
	g = ;
	tabooSearch(g, -1);

	free(matrix);
	free(counter);
	free(size);

	return 0;
}
