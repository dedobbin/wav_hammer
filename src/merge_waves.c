#include "merge_waves.h"
#include "wave_samples.h"
#include <dirent.h> 

int random(int min, int max)
{
	if (min < 0)
		min = 0;
	if (max < min)
		max = min;
	int result  = (double)rand() / (RAND_MAX + 1) * (max - min) + min;
	//printf("%d\t(%d) \n", result, num_samples(fileOne));
	return result;
}

void random_sort_list(char * list[], int list_size)
{
	int i;
	srand(time(NULL));

	for (i = 0; i < 100; i++) {
		int src = random(0, list_size - 1);
		int dst = random(0, list_size - 1);

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

int create_file_list(char * dstList[], const int n, char * path)
{
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (((strcmp(ent->d_name, ".", 1) == 0) && strlen(ent->d_name) == 1)
				|| ((strcmp(ent->d_name, "..", 2) == 0) && strlen(ent->d_name) == 2)) {
				continue;
			}
			else {
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
		closedir(dir);
		return i;
	} else {
		return -1;
	}
}

Raw_wave * merge_waves_random_autovalues(char * path)
{
	merge_waves(path, 10000, 60000, 10000, 60000);
}

Raw_wave * merge_waves_random(char * path, int amount_min, int amount_max, int offset_min, int offset_max)
{
	int listSize = MAX_INPUT_FILES;
	char ** list = malloc(MAX_INPUT_FILES);
	listSize = create_file_list(list, listSize, path);
	Raw_wave * container = create_header();
	// if return from create_file_list < 0, failed to create list(invalid directory?)
	if (listSize > 0) {
		random_sort_list(list, listSize);
		srand(time(NULL));
		int i = 0;
		printf("merge_waves: Merging %d waves..\n", listSize);
		for (i = 0; i < listSize; i++) {
			Raw_wave * wave = NULL;
			if (load_wave(&wave, list[i]) < 0)
				continue;
			int srcAmount = random(amount_min, amount_max);
			int srcOffset = random(offset_min, offset_max);
			insert_samples(container, wave, srcAmount, srcOffset, num_samples(container), false);
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