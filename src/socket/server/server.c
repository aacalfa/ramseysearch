/*
 * server.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#include <time.h>
#include <sys/mman.h>

#include "server.h"
#include "graph_utils.h"
#include "dllist.h"
#include "jval.h"
#include "msg.h"
#include "isomorph.h"
#include "clique_count.h"
#include "utils.h"

typedef struct scheduler {
	Dllist counterExamples; /* List of counterexamples */
	int listSize; /* Size of the counterexample list */
	int currCEsize; /* Current best counter example size */
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

/* addCounterExample
 * Add a counterxample to the Scheduler list of counterexamples
 */
int addCounterExample(int *g) {
	Jval ce;
	ce = new_jval_v(g);

	/* Append counterexample to list */
	dll_append(_Scheduler->counterExamples, ce);
	_Scheduler->listSize++;
}

/* parseMessage
 * Reads message from client
 */
static int parseMessage(int newsockfd) {
	char buffer[BUFSIZ];
	int n;

	/* Initialize buffers */
	char *wholeMessage = (char*) malloc(READBUFFERSIZE*sizeof(char));
	memset(wholeMessage, 0, READBUFFERSIZE);
	memset(buffer, 0, BUFSIZ);

	/* Start reading */
	n = read(newsockfd, buffer, BUFSIZ);
	if (n < 0) {
		fprintf(stderr,"Error: failed to read from socket\n");
		return -1;
	}
	/* reading a result message, must call read several times */
	else if(n > 10) {
		/* Discard garbage at the end */
		buffer[n] = '\0';

		/* append to wholeMessage */
		strcat(wholeMessage, buffer);

		/* Clear buffer before reading next chunk */
		memset(buffer, 0, BUFSIZ);
		do {
			n = read(newsockfd, buffer, BUFSIZ);
			if (n < 0) {
				fprintf(stderr,"Error: failed to read from socket\n");
				return -1;
			}
			else {
				/* Discard garbage at the end */
				buffer[n] = '\0';

				/* append to wholeMessage */
				strcat(wholeMessage, buffer);

				/* Clear buffer before reading next chunk */
				memset(buffer, 0, BUFSIZ);
			}
		} while(n > 0);
	}
	/* reading a request message, one read is enough */
	else {
		strcpy(wholeMessage, buffer);
	}
	char* result = strdup(wholeMessage);

	/* Free wholeMessage */
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
	if(*workingSize <= _Scheduler->currCEsize) /* Can send a hint to the client */
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
		fprintf(stderr, "Counterexample successfully received!\n");
		if(gsize > _Scheduler->currCEsize) { /* Found a counterexample */
			/* Update Scheduler */
			_Scheduler->currCEsize = gsize;
			/* clear list and add new counterexample */
			free_dllist(_Scheduler->counterExamples);
			_Scheduler->counterExamples = new_dllist();
			_Scheduler->listSize = 0;
			addCounterExample(g);

			/* Save counterexample into a file */
			SaveGraph(g,gsize, "../../../counterexamples");
		}
		/* Just add new counterexample */
		else if(gsize == _Scheduler->currCEsize) {
				addCounterExample(g);
				SaveGraph(g,gsize, "../../../counterexamples");
		}
	}

	fprintf(stderr,"gsize = %d, clCount = %d, g = %s\n", gsize, clCount, pch);
}

static int denyRequest(int newsockfd) {
	char msg[2];
	msg[0] = DENY;
	msg[1] = '\0';

	int n = write(newsockfd, msg, strlen(msg));
	if (n < 0) {
		fprintf(stderr,"Error: failed to write to socket\n");
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
	if(workingSize <= _Scheduler->currCEsize) { /* Send counterexample */
		asprintf(&hintGraphSize, "%d", _Scheduler->currCEsize);
		/* Pick randomly a graph from counterexamples list */
		srand(time(NULL));
		int idx = rand() % _Scheduler->listSize + 1;
		int curr = 1;
		int *key_g;
		Dllist ptr;
		dll_traverse(ptr,_Scheduler->counterExamples)
		{
			if(curr == idx) {
				key_g = (int *)jval_v(dll_val(ptr));
				break;
			}
			curr++;
		}

		hintGraph = GraphtoChar(key_g, _Scheduler->currCEsize);
		sprintf(cliqueCount, "%d", 0);
	}

	/* Finish building message */
	hintMessage[0] = RESULT;
	strcat(hintMessage, ":");
	strcat(hintMessage, hintGraphSize);
	strcat(hintMessage, ":");
	strcat(hintMessage, cliqueCount);
	strcat(hintMessage, ":");
	strcat(hintMessage, hintGraph);

	/* just checking */
	if(hintGraph == NULL) {
		fprintf(stderr,"Error: failed to build hint message.\n");
		return -1;
	}

	int n = write(newsockfd, hintMessage, strlen(hintMessage));
	if (n < 0) {
		fprintf(stderr,"Error: failed to write to socket\n");
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

		/* Set the Scheduler pointer to be shared */
#ifdef __APPLE__
		_Scheduler = mmap(NULL, sizeof(_Scheduler), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
#else
		_Scheduler = mmap(NULL, sizeof(_Scheduler), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
#endif

		/* Initialize fields */
		_Scheduler->counterExamples = new_dllist();
		_Scheduler->listSize = 0;

		/* Load best graph counterexamples */
		int file_count;
		char **CEfiles = getCounterExamplesFromFolder("../../../counterexamples/more116", &file_count);
		int i;
		for(i = 0; i < file_count; i++) {
			int *g;
			char *fname;
			asprintf(&fname, "../../../counterexamples/more116/%s", CEfiles[i]);
			if(ReadGraph(fname, &g, &(_Scheduler->currCEsize))) {
				fprintf(stderr, "Loaded graph %s successfully!\n", fname);
				addCounterExample(g);
			}
		}
		// free CEfiles */
		for(i = 0; i < _Scheduler->listSize; i++) {
			free(CEfiles[i]);
		}
		free(CEfiles);

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
		fprintf(stderr,"Error: failed to open socket\n");
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
		fprintf(stderr,"Error: failed to bind\n");
		return -1;
	}
	/* handle at most 5 connections */
	listen(sockfd, 5);

	clilen = sizeof(cli_addr);
	while (1) {
	 /* Block until a client connects */
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			fprintf(stderr,"Error: failed to accept\n");
			return -1;
		}

		/* Create process to handle client message */
		int pid = fork();
		if (pid < 0) {
			fprintf(stderr,"Error: failed to fork\n");
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

