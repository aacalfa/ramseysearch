/*
 * client.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#include "graph_utils.h"
#include "msg.h"
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
	
	printf("gsize = %d, clCount = %d, g = %s\n", gsize, clCount, pch);
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
		//Try to do something
	}

	return g;
}

/*
 * Send a counter example or intermediate result to the server. Flag is to indicate the content.
 * feedback is a buffer to store message from the server.
 */
int sendResult(char* hostname, int HOSTPORT, char* MATRIX, char* MATRIXSIZE,
		char* CLIQUECOUNT, char* feedback) {
	int sockfd; // Socket file describer
	int portno; // Port Number
	int n; // Read/Write status flag
	struct sockaddr_in serv_addr;  //Server Address
	struct hostent *server;  // Server
	char readbuffer[READBUFFERSIZE];  //Buffer for message received from server.
	char* msg = (char*) malloc(READBUFFERSIZE * sizeof(char)); //Message to send.

	// Establish Connection to the server.
	if (hostname == NULL) {
		printf("Error: Wrong Hostname!\n");
		return -1;
	}
	portno = HOSTPORT;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error: Fail to open socket\n");
		return -1;
	}
	server = gethostbyname(hostname);
	if (server == NULL) {
		printf("Error: No such host\n");
		return -1;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		printf("Error: Fail to connect\n");
		return -1;
	}

	// Prepare the sending message.
	//Format RESULT:MATRIXSIZE:CLIQUECOUNT:MATRIX
	msg[0] = RESULT;
	msg[1] = '\0';
	strcat(msg, ":");
	strcat(msg, MATRIXSIZE);
	strcat(msg, ":");
	strcat(msg, CLIQUECOUNT);
	strcat(msg, ":");
	strcat(msg, MATRIX);
	printf("%s", msg);
	//Send message
	n = write(sockfd, msg, strlen(msg));
	if (n < 0) {
		printf("Error: Fail to write to socket");
		return -1;
	}
	free(msg);

	// Initialize the read buffer
	bzero(readbuffer, READBUFFERSIZE);
	// Read message from the server.
	n = read(sockfd, readbuffer, READBUFFERSIZE - 1);
	if (n < 0) {
		printf("Error: Fail to read from socket");
	}
	close(sockfd);
	strcpy(feedback, readbuffer);
	return 1;
}

/*
 * Send a request to the server to get a counter example or an intermediate result.
 * feedback is to store the message received from server.
 */
int sendRequest(char* hostname, int HOSTPORT, char* MATRIXSIZE, char* feedback) {
	int sockfd; // Socket file describer
	int portno; // Port Number
	int n; // Read/Write status flag
	struct sockaddr_in serv_addr;  //Server Address
	struct hostent *server;  // Server
	char readbuffer[READBUFFERSIZE];  //Buffer for message received from server.
	char* msg = (char*) malloc(READBUFFERSIZE * sizeof(char)); //Message to send.

	// Establish Connection to the server.
	if (hostname == NULL) {
		printf("Error: Wrong Hostname!\n");
		return -1;
	}
	portno = HOSTPORT;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error: Fail to open socket\n");
		return -1;
	}
	server = gethostbyname(hostname);
	if (server == NULL) {
		printf("Error: No such host\n");
		return -1;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		printf("Error: Fail to connect\n");
		return -1;
	}

	// Prepare the sending message.
	//Format REQUEST:MATRIXSIZE
	msg[0] = REQUEST;
	msg[1] = '\0';
	strcat(msg, ":");
	strcat(msg, MATRIXSIZE);
	printf("%s", msg);
	//Send message
	n = write(sockfd, msg, strlen(msg));
	if (n < 0) {
		printf("Error: Fail to write to socket");
		return -1;
	}
	free(msg);

	// Initialize the read buffer
	bzero(readbuffer, READBUFFERSIZE);
	// Read message from the server.
	n = read(sockfd, readbuffer, READBUFFERSIZE - 1);
	if (n < 0) {
		printf("Error: Fail to read from socket");
	}
	close(sockfd);
	strcpy(feedback, readbuffer);
	return 1;
}
