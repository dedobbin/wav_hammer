#include "merge_waves.h"
#include "wave_samples.h"
#include <dirent.h>

void create_file_list(char * dstList[], int n, char * path)
{
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			//+1 because need extra /
			dstList[i] = malloc(strlen(path) + strlen(ent->d_name) + 1);
			strcpy(dstList[i], path);
			strcat(dstList[i], "/");
			strcat(dstList[i], ent->d_name);
		}
		closedir(dir);
	}
	else {
		return EXIT_FAILURE;
	}
}

Raw_wave * merge_waves()
{
	char * list[5];
	create_file_list(list, 5, "../../audio");
	Raw_wave * container = create_header();
	//void insert_samples(Raw_wave * dst, Raw_wave * src, long amount, long dst_offset, bool overwrite);
	Raw_wave * fileOne;
	load_wave(&fileOne, list[0]);
	insert_samples(container, fileOne,num_samples(fileOne), 0, false);
	
	return container;
}