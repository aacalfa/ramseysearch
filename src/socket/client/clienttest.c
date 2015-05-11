#include <limits.h>
#include <time.h>

#include "msg.h"
#include "client.h"
#include "graph_utils.h"
#include "utils.h"
#include "taboo_search_client.h"

int main(int argc, char *argv[]) {
	char feedback[READBUFFERSIZE];
	int *g = NULL; /* graph adjacency matrix */
	int gsize = 0; /* initialize working graph size */
	int gccounter = INT_MAX; /* initialize clique count */
	char* size;

	/* Convert gsize to string */
	size = NumtoString(gsize);

	/* Contact server and get counterexample to be used as starting point */
	time_t t = time(NULL);
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	fprintf(stderr, "Hello, %s! Local time is: %s\n", hostname, asctime(localtime(&t)));
	fprintf(stderr, "Attempting to connect to server: %s ...\n", SERVERNAME, asctime(localtime(&t)));
	
	bzero(feedback, READBUFFERSIZE);
	if(sendRequest(HOSTNAME, SERVERPORT, size, feedback) == 1)
		fprintf(stderr, "Connection established!\n");
	else
		fprintf(stderr, "Connection could not be established! Starting the search from small graph size.\n");


	int newSize;
	int newCount;
	
	g = parseMessage(feedback, &newSize, &newCount);

	fprintf(stderr, "Starting point is: gsize = %d cliquecount = %d\n", newSize, newCount);

	/* Start the search */
	tabooSearch(g, newSize);

	free(size);

	return 0;
}
