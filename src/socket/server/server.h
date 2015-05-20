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

#define MATRIXMAXSIZE 210*210

int waitForMessage(void);
int initializeScheduler(char *ce, char *in);

#endif /* SERVER_SERVER_H_ */
