/*
 * runserver.c
 *
 *  Created on: Apr 30, 2015
 *      Author: geurney
 */
#include "server.h"
int main(int argc, char *argv[]) {
	char* result = (char*)malloc(MATRIXMAXSIZE*sizeof(char));

	receiveCounterExample(result);
	free(result);
	return 0;
}
