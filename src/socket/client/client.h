/*
 * client.h
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
* Send a counter example or intermediate result to the server.
*/
int sendResult(char* hostname, int HOSTPORT, char* MATRIX, char* MATRIXSIZE, char* CLIQUECOUNT, char* feedback);

/*
* Send a request to the server to get a counter example or an intermediate result.
*/
int sendRequest(char* hostname, int HOSTPORT, char* MATRIXSIZE, char* feedback);

/*
* Parse the result received from the server.
*/
int* parseResult(char *pch, int* gsize, int* clCount);
/*
* Parse the feedback received from the server.
*/
int* parseMessage(char* msg, int* gsize, int* clCount);
#endif /* CLIENT_H_ */
