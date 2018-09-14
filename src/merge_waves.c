#include "merge_waves.h"
#include "wave_samples.h"
#include "utils.h"
#include <stdlib.h>
#include <dirent.h> 
#include <string.h>
#include <time.h>

void random_sort_list(char * list[], int list_size)
{
	int i;
	srand(time(NULL));

	for (i = 0; i < 100; i++) {
		int src = my_random(0, list_size - 1);
		int dst = my_random(0, list_size - 1);

		char * tmp = malloc(strlen(list[dst]) + 1);
		strcpy(tmp, list[dst]);

		free(list[dst]);
		list[dst] = malloc(strlen(list[src]) + 1);
		strcpy(list[dst], list[src]);

		free(list[src]);
		list[src] = malloc(strlen(tmp) + 1);
		strcpy(list[src], tmp);

		free(tmp);
	}
}

int create_file_list(char * dstList[], const int n, char * path, int times)
{
	if (times < 1) times = 1;

	DIR *dir;
	struct dirent *ent;
	int i = 0, j = 0;
	for (j = 0; j < times; j++) {
		if ((dir = opendir(path)) != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				if (((strcmp(ent->d_name, ".") == 0) && strlen(ent->d_name) == 1)
					|| ((strcmp(ent->d_name, "..") == 0) && strlen(ent->d_name) == 2)) {
					continue;
				} else {
					//+1 because need extra slash, +1 for \0
					dstList[i] = malloc(strlen(path) + strlen(ent->d_name) + 2);
					strcpy(dstList[i], path);
					strcat(dstList[i], "/");
					strcat(dstList[i], ent->d_name);
					i++;
					if (i >= n)
						break;
				}
			}
		} else {
			return -1;
		}
	}
	closedir(dir);
	return i;
}

Raw_wave * merge_waves_random_autovalues(char * path)
{
	merge_waves_random(path, 10000, 60000, 10000, 60000, 30, 1);
}

Raw_wave * merge_waves_random(char * path, int amount_min, int amount_max, int offset_min, int offset_max, int perc_skip, int times)
{
	int listSize = MAX_INPUT_FILES;
	char ** list = malloc(MAX_INPUT_FILES);
	listSize = create_file_list(list, listSize, path, times);
	Raw_wave * container = create_header(44800, 16);

	// if return from create_file_list < 0, failed to create list(invalid directory?)
	if (listSize > 0) {
		random_sort_list(list, listSize);
		srand(time(NULL));
		int i = 0;
		printf("merge_waves: Merging %d waves..\n", listSize);
		for (i = 0; i < listSize; i++) {
			if (perc_skip > 0 && my_random(1, 100) < perc_skip) {
				printf("skip %d\n", i);
				continue;
			}
			Raw_wave * wave = NULL;
			if (load_wave(&wave, list[i]) < 0)
				continue;
			int srcAmount = my_random(amount_min, amount_max <= 0 ? num_samples(wave) : amount_max);
			int srcOffset = my_random(offset_min, offset_max);
			insert_samples(.dst = container, .src = wave, .src_amount = srcAmount, .src_offset = srcOffset);
			destroy_wave(&wave);
		}
	} else {
		printf("merge_waves: Failed to merge waves: Couldn't create filelist\n");
	}
	//cleanup list
	if (listSize > 0) {
		int i;
		for (i = 0; i < listSize; i++) {
			if (list[i])
				free(list[i]);
		}
		free(list);
	}
	return container;
}
