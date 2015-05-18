#include <limits.h>
#include <time.h>

#include "msg.h"
#include "client.h"
#include "graph_utils.h"
#include "utils.h"
#include "taboo_search_client.h"
#include "clique_count.h"

int main(int argc, char *argv[]) {
	char *feedback = NULL;
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
	fprintf(stderr, "Attempting to connect to server: %s ...\n", SERVERNAME);

  feedback = sendRequest(size);
  
	if(feedback != NULL)
		fprintf(stderr, "Connection established!\n");
	else
		fprintf(stderr, "Connection could not be established! Starting the search from small graph size.\n");

	int newSize = 0;
	int newCount = 0;
	g = parseMessage(feedback, &newSize, &newCount);

	/* Free memory */
	free(feedback);
	free(size);

	fprintf(stderr, "Starting point is: gsize = %d cliquecount = %d\n", newSize, newCount);

	/* Start the search */
	tabooSearch(g, newSize);

	return 0;
}
