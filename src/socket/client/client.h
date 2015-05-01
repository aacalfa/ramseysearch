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

static int READBUFFERSIZE = 256;
static int SERVERPORT = 5098;
int sendCounterExample(char* HOSTNAME, int HOSTPORT, char* MATRIX, char* result);


#endif /* CLIENT_H_ */
