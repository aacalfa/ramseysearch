/*
 * server.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#include "server.h"
#include "graph_utils.h"
#include "dllist.h"
#include "jval.h"
#include "msg.h"
#include "clique_count.h"

typedef struct scheduler {
	int *currCE; /* Current best counter example found */
	int currCEsize; /* Current best counter example size */

	int *currIN; /* Current best intermediate graph found */
	int currINsize; /* Current best intermediate graph size */
	int currINclcount; /* Current best intermediate result clique count */

	Dllist clients; /* List of all clients currently connected */
}Scheduler;

typedef struct clientnode {
	int sockfd;
	struct sockaddr_in cli_addr;
}ClientNode;

/* Scheduler singleton instance */
Scheduler *_Scheduler = NULL;


static void parseResult(char *pch);
static int parseRequest(char *pch, int *workingSize);
static int sendHint(int newsockfd, int workingSize);
static int denyRequest(int newsockfd);

/*********************
 _                 _ 
| |   ___  __ __ _| |
| |__/ _ \/ _/ _` | |
|____\___/\__\__,_|_|
*********************/

/* addClient
 * Add a client to the Scheduler list of clients
 */
int addClient(int clisockfd, struct sockaddr_in cli_addr) {
	/* Create ClientNode and Jval data representing the client */
	ClientNode *clnode = (ClientNode*) malloc(sizeof(ClientNode));
	Jval client;
	client = new_jval_v(clnode);

	/* Append client to list */
	dll_append(_Scheduler->clients, client);
}

/* parseMessage
 * Reads message from client and sends acknowledgement to client
 */
static int parseMessage(int newsockfd) {
	char buffer[BUFSIZ];
	int n;

	/* Puts message into buffer */
	char *wholeMessage = (char*) malloc(READBUFFERSIZE*sizeof(char));
	bzero(buffer, BUFSIZ);
	n = read(newsockfd, buffer, BUFSIZ);
	if (n < 0) {
		printf("Error: failed to read from socket\n");
		return -1;
	}
	/* reading a result message, must call read several times */
	else if(n > 10) {
		/* append to wholeMessage */
		strcat(wholeMessage, buffer);
		do {
			n = read(newsockfd, buffer, BUFSIZ);
			if (n < 0) {
				printf("Error: failed to read from socket\n");
				return -1;
			}
			else {
				/* append to wholeMessage */
				strcat(wholeMessage, buffer);
			}
		} while(n > 0);
	}
	/* reading a request message, one read is enough */
	else {
		strcpy(wholeMessage, buffer);
	}
	char* result = (char*)malloc(strlen(buffer)*sizeof(char));
	result = strdup(wholeMessage);

	/* Free result */
	free(wholeMessage);

	/* Parse message from client */
	/* Check first digit of message and verify if it is a
	 * a result or a request */
	char *pch;
	pch = strtok(result, ":");
	if(pch[0] == RESULT) {
		/* Parse rest of the message */
		parseResult(pch);
	}
	else if(pch[0] == REQUEST) {
		/* Send hint to the client */
		int workingSize;
		int hint = parseRequest(pch, &workingSize);
		int ret;
		if(hint)
			ret = sendHint(newsockfd, workingSize);
		else
			ret = denyRequest(newsockfd);
	}

	close(newsockfd);

	/* Free memory */
	free(result);
	return 1;
}

/* parseRequest
 * Parses a message containing a request message from client
 * Message format "[requestflag]:[working graph size]"
 * Returns 1 if a hint should be sent to the client.
 * Returns 0 if no good hint can be provided.
 */
static int parseRequest(char *pch, int *workingSize) {
	/* Get working size */
	pch = strtok(NULL, ":");
	*workingSize = atoi(pch);

	/* Decide whether to send a hint or not */
	if(*workingSize < _Scheduler->currCEsize) /* Can send a hint to the client */
		return 1;
	else/* Found an intermediate result */  
		return 0;
}

/* parseResult
 * Parses a message containing the graph adjacency matrix
 * Message format "[graphsize]:[graph adjacency matrix]"
 */
static void parseResult(char *pch) {
	/* Get gsize */
	pch = strtok(NULL, ":");
	int gsize = atoi(pch);

	/* Get Clique Count */
	pch = strtok(NULL, ":");
	int clCount = atoi(pch);

	/* Get matrix */
	pch = strtok(NULL, ":");
	int *g = ChartoGraph(pch, gsize);

	/* Verify integrity of g */
	int realCount = CliqueCount(g, gsize);

	/* Message is invalid */
	if (realCount != clCount) {
		fprintf(stderr, "Message could not be validated!\n");
		fprintf(stderr, "Clique count from message: %d, actual clique count: %d!\n", clCount, realCount);
		return;
	}

	/* Update scheduler */
	if(clCount == 0) {
		if(gsize > _Scheduler->currCEsize) { /* Found a counterexample */
			fprintf(stderr, "Counterexample successfully received!\n");
			_Scheduler->currCEsize = gsize;
			_Scheduler->currCE = g;
		}
	}
	else if (clCount < _Scheduler->currINclcount || gsize > _Scheduler->currINsize) {/* Found an intermediate result */  
		fprintf(stderr, "Intermediate result successfully received!\n");
		_Scheduler->currINclcount = clCount;
		_Scheduler->currINsize = gsize;
		_Scheduler->currIN = g;
	}

	printf("gsize = %d, clCount = %d, g = %s\n", gsize, clCount, pch);
}

static int denyRequest(int newsockfd) {
	char msg[2];
	msg[0] = DENY;
	msg[1] = '\0';

	int n = write(newsockfd, msg, strlen(msg));
	if (n < 0) {
		printf("Error: failed to write to socket\n");
		return -1;
	}
}

static int sendHint(int newsockfd, int workingSize) {
	/* Initialize hint */
	char* hintGraph = NULL;
	char *hintGraphSize = NULL;
	char hintMessage[READBUFFERSIZE];
	char cliqueCount[BUFSIZ];

	/* Hint message structure: [result flag]:[matrixsize]:[cliquecount]:[graphmatrix] */

	/* Decide which hint to send */
	if(workingSize < _Scheduler->currCEsize) { /* Send counterexample */
		asprintf(&hintGraphSize, "%d", _Scheduler->currCEsize);
		hintGraph = GraphtoChar(_Scheduler->currCE, _Scheduler->currCEsize);
		sprintf(cliqueCount, "%d", 0);
	}
	else { /* Send intermediate */
		asprintf(&hintGraphSize, "%d", _Scheduler->currINsize);
		hintGraph = GraphtoChar(_Scheduler->currIN, _Scheduler->currINsize);
		sprintf(cliqueCount, "%d", _Scheduler->currINclcount);
	}
	/* Finish building message */
	hintMessage[0] = RESULT;
	hintMessage[1] = '\0';
	strcat(hintMessage, ":");
	strcat(hintMessage, hintGraphSize);
	strcat(hintMessage, ":");
	strcat(hintMessage, cliqueCount);
	strcat(hintMessage, ":");
	strcat(hintMessage, hintGraph);
	
	/* just checking */
	if(hintGraph == NULL) {
		printf("Error: failed to build hint message.\n");
		return -1;
	}

	int n = write(newsockfd, hintMessage, strlen(hintMessage));
	if (n < 0) {
		printf("Error: failed to write to socket\n");
		return -1;
	}
	/* Free memory */
	free(hintGraph);
}

/**************************
  ___ _        _          _ 
 / __| |   ___| |__  __ _| |
| (_ | |__/ _ \ '_ \/ _` | |
 \___|____\___/_.__/\__,_|_|
**************************/

/* initializeScheduler
 * Initializes scheduler if it has not been initialized yet.
 * Returns 0 if the scheduler was initialized correctly.
 * Returns 1 if the scheduler already exists.
 * Returns -1 if the initialization was unsuccessful.
 */
int initializeScheduler(void) {
	if(_Scheduler == NULL) {
		_Scheduler = (Scheduler*) malloc(sizeof(Scheduler));
		if(_Scheduler == NULL)
			return -1;

		/* Initialize fields */
		_Scheduler->clients = new_dllist();
		/* Load best graph counterexample */
		ReadGraph("../../../counterexamples/n111.txt", &(_Scheduler->currCE), &(_Scheduler->currCEsize));
//		ReadGraph("../../../counterexamples/n8.txt", &(_Scheduler->currCE), &(_Scheduler->currCEsize));
		/* Load best intermediate counterexample */
		ReadGraph("../../../intermediate/n112.txt", &(_Scheduler->currIN), &(_Scheduler->currINsize));
		return 0;
	}
	else
		return 1;
}

/*
 * Server listens and waits for messages from clients.
 */
int waitForMessage(void) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	/* Create a new socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error: failed to open socket\n");
		return -1;
	}

	/* Initialize server address structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = SERVERPORT;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); /* convert port number to network byte order */

	/* Attempt to bind the socket to the host address */
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error: failed to bind\n");
		return -1;
	}
	/* handle at most 5 connections */
	listen(sockfd, 5);

	clilen = sizeof(cli_addr);
	while (1) {
	 /* Block until a client connects */
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			printf("Error: failed to accept\n");
			return -1;
		}

		/* Create process to handle client message */
		int pid = fork();
		if (pid < 0) {
			printf("Error: failed to fork\n");
		}
		if (pid == 0)  {
			close(sockfd);
			/* Parse message from client */
			parseMessage(newsockfd);
			exit(0);
		}
		else {
			close(newsockfd);
		} 
	}
	close(sockfd);
	return 1;
}

