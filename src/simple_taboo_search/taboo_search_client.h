#ifndef TABOO_SEARCH_H_
#define TABOO_SEARCH_H_


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define EDGEONLY

#define MAXSIZE (541)

#define TABOOSIZE (500)
#define BIGCOUNT (9999999)
int tabooSearch(int *matrix, int matrixSize);


#endif /* TABOO_SEARCH_H_ */