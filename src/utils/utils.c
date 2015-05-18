/**
 * Convert a number to string.
 */
#include <stdlib.h>
char* NumtoString(int num)
{
	int len;
	if(num > 99) {
		len = 3;
	} else if(num > 9) {
		len = 2;
	} else {
		len = 1;
	}
	char* size = (char*)malloc((len+1)*sizeof(char));
	
	int i = len - 1;
	int rem;
	if (num == 0) {
		size[0] = '0';
	} else {
		while(num > 0) {
			rem = num % 10;
			num = num / 10;
			size[i--] = rem + '0';
		}
	}
	size[len] = '\0';
	return size;
}
