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
	int currINsize; /* Current best counter example size */

	Dllist clients; /* List of all clients currently connected */
}Scheduler;

typedef struct clientnode {
	int sockfd;
	struct sockaddr_in cli_addr;
}ClientNode;

/* Scheduler singleton instance */
Scheduler *_Scheduler = NULL;

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

/* receiveservice
 * Reads message from client and sends acknowledgement to client
 */
static int receiveservice(int newsockfd, char* result) {
	char buffer[MATRIXMAXSIZE];
	int n;

	/* Puts message into buffer */
	bzero(buffer, MATRIXMAXSIZE);
	n = read(newsockfd, buffer, MATRIXMAXSIZE);
	if (n < 0) {
		printf("Error: failed to read from socket\n");
		return -1;
	}
	strcpy(result, buffer);

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

/* parseMatrix
 * Parses a message containing the graph adjacency matrix
 * Message format "[graphsize]:[graph adjacency matrix]"
 */
static void parseMatrix(char *matrix, int **g, int *gsize) {
	char *pch;
	pch = strtok(matrix, ":");
	*gsize = atoi(pch);
	pch = strtok(NULL, ":");
	*g = ChartoGraph(pch, *gsize);
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
 * Receives a counterExample graph from a client.
 */
int receiveCounterExample(char* matrix) {
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
			int *g;
			int gsize;
			close(sockfd);
			/* Parse message from client */
			receiveservice(newsockfd, matrix);
			printf("%s\n", matrix);
		//	parseMatrix(matrix, &g, &gsize);
	//		PrintGraph(g, gsize);
			exit(0);
		}
		else {
			close(newsockfd);
		} 
	}
	close(sockfd);
	return 1;
}

