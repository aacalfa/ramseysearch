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
	char buffer[MATRIXMAXSIZE];
	int n;

	/* Puts message into buffer */
	bzero(buffer, MATRIXMAXSIZE);
	n = read(newsockfd, buffer, MATRIXMAXSIZE);
	if (n < 0) {
		printf("Error: failed to read from socket\n");
		return -1;
	}
	char* result = (char*)malloc(strlen(buffer)*sizeof(char));
	strcpy(result, buffer);

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
		
	}

	/* Send ack to client */
	char* ack = (char*) malloc(100 * sizeof(char));
	strcpy(ack, SERVERNAME);
	strcat(ack, " got your message!");
	n = write(newsockfd, ack, strlen(ack));
	if (n < 0) {
		printf("Error: failed to write to socket\n");
		return -1;
	}
	free(ack);
	close(newsockfd);
	return 1;
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

	/* Update scheduler */
	if(clCount == 0) {
		if(gsize > _Scheduler->currCEsize) { /* Found a counterexample */
			_Scheduler->currCEsize = gsize;
			_Scheduler->currCE = g;
		}
	}
	else if (clCount < _Scheduler->currINclcount || gsize > _Scheduler->currINsize) {/* Found an intermediate result */  
		_Scheduler->currINclcount = clCount;
		_Scheduler->currINsize = gsize;
		_Scheduler->currIN = g;
	}

	printf("gsize = %d, clCount = %d, g = %s\n", gsize, clCount, pch);
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

