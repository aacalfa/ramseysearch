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

static int SERVERPORT = 5098;
static char* SERVERNAME = "Geurney";
static int MATRIXMAXSIZE = 200*200;

int receiveCounterExample(char* matrix);

#endif /* SERVER_SERVER_H_ */
