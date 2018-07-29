#include "merge_waves.h"
#include <dirent.h>

void create_file_list(FILE * dstList[], int n, char * path)
{
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			printf("%s\n", ent->d_name);
			//+1 because need extra /
			char * filepath = malloc(strlen(path) + strlen(ent->d_name)+1);
			strcpy(filepath, path);
			strcat(filepath, "/");
			strcat(filepath, ent->d_name);
			FILE * f = fopen(filepath, "rb");
#ifdef __linux__ 
			if (!f || access(path, R_OK)) {
				fprintf(stderr, "create_file_list: Could not open file '%s' for reading\n", path);
				continue;
			}
#elif _WIN32
			if ((_access(path, 0) == -1)) {
				fprintf(stderr, "create_file_list: File '%s' does not exist\n", path);
				continue;
			}
#endif
			dstList[i++] = f;
		}
		closedir(dir);
	}
	else {
		return EXIT_FAILURE;
	}
}

void merge_waves()
{
	FILE * list[5];
	create_file_list(list, 5, "../../audio");
	return;
}