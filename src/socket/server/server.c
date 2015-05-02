/*
 * server.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#include "server.h"
#include "graph_utils.h"
int receiveservice(int newsockfd, char* result) {
	char buffer[MATRIXMAXSIZE];
	int n;
	bzero(buffer, MATRIXMAXSIZE);
	n = read(newsockfd, buffer, MATRIXMAXSIZE);
	if (n < 0) {
		printf("Error: failed to read from socket\n");
		return -1;
	}
	strcpy(result, buffer);

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
}

void parseMatrix(char *matrix, int **g, int *gsize) {
	char *pch;
	pch = strtok(matrix, ":");
	*gsize = atoi(pch);
	pch = strtok(NULL, ":");
	*g = ChartoGraph(pch, *gsize);
}

int receiveCounterExample(char* matrix) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error: failed to open socket\n");
		return -1;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = SERVERPORT;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error: failed to bind\n");
		return -1;
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			printf("Error: failed to accept\n");
			return -1;
		}
		int pid = fork();
		if (pid < 0) {
			printf("Error: failed to fork\n");
		}
		if (pid == 0)  {
			int *g;
			int gsize;
             		close(sockfd);
             		receiveservice(newsockfd, matrix);
			parseMatrix(matrix, &g, &gsize);
			PrintGraph(g, gsize);
             		exit(0);
         	}
         	else {
         		close(newsockfd);
		} 
	}
	close(sockfd);
	return 1;
}

