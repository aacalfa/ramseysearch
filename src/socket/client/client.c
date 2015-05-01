/*
 * client.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#include "client.h"

int sendCounterExample(char* HOSTNAME, int HOSTPORT, char* MATRIX, char* result) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char readbuffer[READBUFFERSIZE];

	if (HOSTNAME == NULL) {
		printf("Error: Wrong Hostname!\n");
		return -1;
	}
	portno = HOSTPORT;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error: Fail to open socket\n");
		return -1;
	}
	server = gethostbyname(HOSTNAME);
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

	n = write(sockfd, MATRIX, strlen(MATRIX));
	if (n < 0) {
		printf("Error: Fail to write to socket");
		return -1;
	}

	bzero(readbuffer, READBUFFERSIZE);
	n = read(sockfd, readbuffer, READBUFFERSIZE - 1);
	if (n < 0) {
		printf("Error: Fail to read from socket");
	}
	close(sockfd);
	strcpy(result, readbuffer);
	return 1;
}
