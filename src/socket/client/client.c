/*
 * client.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

//#include "graph_utils.h"
#include "../../utils/graph_utils.h"
//#include "msg.h"
#include "../../utils/msg.h"
#include "client.h"

int* parseResult(char *pch, int* gsize, int* clCount) {
	/* Get gsize */
	pch = strtok(NULL, ":");
	*gsize = atoi(pch);

	/* Get Clique Count */
	pch = strtok(NULL, ":");
	*clCount = atoi(pch);

	/* Get matrix */
	pch = strtok(NULL, ":");
	int *g = ChartoGraph(pch, *gsize);
	
	return g;
}

int* parseMessage(char* msg, int* gsize, int* clCount) {
	/* Parse message from Server */
	/* Check first digit of message and verify if it is a
	 * a deny or a request */
	int* g = NULL;
	char* pch = strtok(msg, ":");
	if(msg[0] == RESULT) {
		/* Parse rest of the message */
		g = parseResult(msg, gsize, clCount);
	}
	else if(msg[0] == DENY) {
		/* Try to do something */
	}

	return g;
}

/*
 * Send a counter example or intermediate result to the server. Flag is to indicate the content.
 * feedback is a buffer to store message from the server.
 */
int sendResult(char* MATRIX, char* MATRIXSIZE, char* CLIQUECOUNT) {
	int sockfd; /* Socket file describer */
	int portno; /* Port Number */
	int n; /* Read/Write status flag */
	struct sockaddr_in serv_addr;  /* Server Address */
	struct hostent *server;  /* Server */
	char* msg = (char*) malloc(READBUFFERSIZE * sizeof(char)); /* Message to send. */
  int i = 0;
  int trytime = 0;
  char *hostname = NULL;
  char *hostnames[3]; 
  hostnames[0] = HOSTNAME1;
  hostnames[1] = HOSTNAME2;
  hostnames[2] = HOSTNAME3;
  
	/* Initialize msg */
	memset(msg, 0, READBUFFERSIZE);

	/* Establish Connection to the server. */
  do {
      hostname = hostnames[i++];
  	if (hostname == NULL) {
  		fprintf(stderr,"Error: Hostname is NULL!\n");
  		continue;
  	}
  	portno = SERVERPORT;
  	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  	if (sockfd < 0) {
  		fprintf(stderr,"Error: Fail to open socket\n");
  		continue;
  	}
  	server = gethostbyname(hostname);
  	if (server == NULL) {
  		fprintf(stderr,"Error: No such host: %s \n", hostname);
  		continue;
  	}
  	bzero((char *) &serv_addr, sizeof(serv_addr));
  	serv_addr.sin_family = AF_INET;
  	bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
  	serv_addr.sin_port = htons(portno);
  	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
  			< 0) {
  		fprintf(stderr,"Error: Fail to connect %s\n", hostname);
      if(i >= 3) {
        fprintf(stderr, "Error: All Server failed!\n");
        return -1;
      }
  	} else {
      fprintf(stderr,"Connected to %s\n", hostname);
      break;
    }
 }while(i < 3);

	/* Prepare the sending message. */
	/* Format RESULT:MATRIXSIZE:CLIQUECOUNT:MATRIX */
	msg[0] = RESULT;
	strcat(msg, ":");
	strcat(msg, MATRIXSIZE);
	strcat(msg, ":");
	strcat(msg, CLIQUECOUNT);
	strcat(msg, ":");
	strcat(msg, MATRIX);

	/* Send message */
	n = write(sockfd, msg, strlen(msg));
	if (n < 0) {
		fprintf(stderr,"Error: Fail to write to socket");
		return -1;
	}
	free(msg);

	close(sockfd);
	return 1;
}

/*
 * Send a request to the server to get a counter example or an intermediate result.
 * feedback is to store the message received from server.
 */
char* sendRequest(char* MATRIXSIZE) {
	int sockfd; /* Socket file describer */
	int portno; /* Port Number */
	int n; /* Read/Write status flag */
	struct sockaddr_in serv_addr;  /* Server Address */
	struct hostent *server;  /* Server */
	char readbuffer[BUFSIZ];  /* Buffer for message received from server. */
	char* msg = (char*) malloc(READBUFFERSIZE * sizeof(char)); /* Message to send. */
  int i = 0;
  int trytime = 0;
  char *hostname = NULL;
  char *hostnames[3]; 
  hostnames[0] = HOSTNAME1;
  hostnames[1] = HOSTNAME2;
  hostnames[2] = HOSTNAME3;
  
	/* Establish Connection to the server. */
  do {
      hostname = hostnames[i++];
  	if (hostname == NULL) {
  		fprintf(stderr,"Error: Hostname is NULL!\n");
  		continue;
  	}
  	portno = SERVERPORT;
  	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  	if (sockfd < 0) {
  		fprintf(stderr,"Error: Fail to open socket\n");
  		continue;
  	}
  	server = gethostbyname(hostname);
  	if (server == NULL) {
  		fprintf(stderr,"Error: No such host: %s \n", hostname);
  		continue;
  	}
  	bzero((char *) &serv_addr, sizeof(serv_addr));
  	serv_addr.sin_family = AF_INET;
  	bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
  	serv_addr.sin_port = htons(portno);
  	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
  			< 0) {
  		fprintf(stderr,"Error: Fail to connect %s\n", hostname);
      if(i >= 3) {
        fprintf(stderr, "Error: All Server failed!\n");
        return NULL;
      }
  	} else {
      fprintf(stderr,"Connected to %s\n", hostname);
      break;
    }
 }while(i < 3);

	/* Prepare the sending message. */
	/* Format REQUEST:MATRIXSIZE */
	msg[0] = REQUEST;
	strcat(msg, ":");
	strcat(msg, MATRIXSIZE);

	/* Send message */
	n = write(sockfd, msg, strlen(msg));
	if (n < 0) {
		fprintf(stderr,"Error: Fail to write to socket");
		return NULL;
	}
	free(msg);

	/* Initialize the read buffer */
	memset(readbuffer, 0, BUFSIZ);

	/* Because the message sent from the server can be large,
	 * we must read it in chunks. wholeMessage will aggregate
	 * all packets sent from the server.
	 */
	char *wholeMessage = (char*) malloc(READBUFFERSIZE*sizeof(char));
	memset(wholeMessage, 0, READBUFFERSIZE);

	/* Read message from the server. */
	do {
		n = read(sockfd, readbuffer, BUFSIZ);
		if (n < 0)
			fprintf(stderr,"Error: Fail to read from socket");
		else {
			/* Discard garbage at the end */
			readbuffer[n] = '\0';
			/* append to wholeMessage */
			strcat(wholeMessage, readbuffer);
			/* Clear buffer before reading next chunk */
			memset(readbuffer, 0, BUFSIZ);
		}
	}while(n > 0);

	close(sockfd);
	char* feedback = (char*)malloc(strlen(readbuffer)*sizeof(char));
	feedback = strdup(wholeMessage);

	/* Free memory */
	free(wholeMessage);

	return feedback;
}
