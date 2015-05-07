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
static int READBUFFERSIZE = 220*220;

/*
* Fixed Server Port.
*/
static int SERVERPORT = 5098;

/*
* Flag of message.
*/
static char* COUNTEREXAMPLE = "0";
static char* INTERMEDIATERESULT = "1";
static char* REQUEST = "2";

/*
* Send a counter example or intermediate result to the server.
*/
int sendResult(char* HOSTNAME, int HOSTPORT, char* flag, char* MATRIX, char* MATRIXSIZE, char* feedback);

/*
* Send a request to the server to get a counter example or an intermediate result.
*/
int sendRequest(char* HOSTNAME, int HOSTPORT, char* feedback);

#endif /* CLIENT_H_ */
