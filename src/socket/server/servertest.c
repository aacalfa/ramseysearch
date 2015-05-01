#include "client.h"

int main(int argc, char *argv[]) {
	char* hostname;
	char* matrix;
	char result[READBUFFERSIZE];

	hostname = "localhost";
	matrix = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";

	if(sendCounterExample(hostname, SERVERPORT, matrix, result)) {
		printf("%s\n", result);
	}
	return 0;
}
