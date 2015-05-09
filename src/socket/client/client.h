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
* The size of read buffer. As large as the matrix size.
*/
#define READBUFFERSIZE 220*220

/*
* Fixed Server Port.
*/
#define SERVERPORT 5098

/*
* Flag of message.
*/
#define RESULT '0'
#define REQUEST '1'

/*
* Send a counter example or intermediate result to the server.
*/
int sendResult(char* HOSTNAME, int HOSTPORT, char* MATRIX, char* MATRIXSIZE, char* CLIQUECOUNT, char* feedback);

/*
* Send a request to the server to get a counter example or an intermediate result.
*/
int sendRequest(char* HOSTNAME, int HOSTPORT, char* feedback);

#endif /* CLIENT_H_ */
