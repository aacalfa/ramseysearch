/*
 * server.h
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVERPORT 5098
#define SERVERNAME "Geurney"
#define MATRIXMAXSIZE 210*210

/*
* The size of read buffer. As large as the matrix size.
*/
#define READBUFFERSIZE 220*220

/*
* Flag of message.
*/
#define RESULT '0'
#define REQUEST '1'

int waitForMessage(void);
int initializeScheduler(void);

#endif /* SERVER_SERVER_H_ */
