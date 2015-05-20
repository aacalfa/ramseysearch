/*
 * runserver.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */
#include "server.h"

int main(int argc, char *argv[]) {

	/* verify if number of parameters are correct
	 * must specify both counterexample and intermediate
	 * result, example: ./server n8.txt n9.txt
	 */
	if(argc != 3) {
		puts("Usage: ./server [filename of counterexample] [filename of intermediate]");
		exit(1);
	}

	/* Initialize Scheduler passing counterexample and intermediate graphs */
	initializeScheduler(argv[1], argv[2]);

	/* Run server */
	waitForMessage();
	return 0;
}
