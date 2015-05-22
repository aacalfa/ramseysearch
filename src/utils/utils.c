/**
 * Convert a number to string.
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

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

char** getCounterExamplesFromFolder(char *path, int *file_count) {
	DIR *dir;
	struct dirent *ent;

	*file_count = 0;
	/* Count how many counterexamples are there */
	dir = opendir(path);
	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL) { /* If the entry is a regular file and is a text file */
			(*file_count)++;
		}
	}
	closedir(dir);

	/* Store names in an array of strings */
	char **counterExamples;
	counterExamples = (char **) malloc((*file_count) * sizeof(char*));
	for (int j = 0; j < (*file_count); j++)
		counterExamples[j] = malloc(BUFSIZ * sizeof(char));

	int i = 0;

	/* Now get the names */
	if ((dir = opendir (path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL) { /* If the entry is a regular file and is a text file */
				strcpy(counterExamples[i],ent->d_name);
				i++;
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return NULL;
	}
	return counterExamples;
}
