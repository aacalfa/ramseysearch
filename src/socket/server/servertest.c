/*
 * runserver.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */
#include "server.h"
int main(int argc, char *argv[]) {
	initializeScheduler();
	waitForMessage();
	return 0;
}
