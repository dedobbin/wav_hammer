#include "merge_waves.h"
#include "wave_samples.h"
#include <dirent.h>


int random(int min, int max)
{
	int result  = (double)rand() / (RAND_MAX + 1) * (max - min) + min;
	//printf("%d\t(%d) \n", result, num_samples(fileOne));
	return result;
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
				//+1 because need extra slash
				dstList[i] = malloc(strlen(path) + strlen(ent->d_name) + 1);
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
	}
	else {
		return -1;
	}
}

Raw_wave * merge_waves()
{
	int listSize = MAX_INPUT_FILES;
	char * list[MAX_INPUT_FILES];
	listSize = create_file_list(list, listSize, "../../audio");
	Raw_wave * container = create_header();
	
	srand(time(NULL));
	int i = 0;
	for (i = 0; i < listSize; i++) {
		Raw_wave * wave = NULL;
		load_wave(&wave, list[i]);
		int srcAmount = random(10000, 60000);
		int srcOffset = random(10000, num_samples(wave));
		insert_samples(container, wave, srcAmount, srcOffset, num_samples(container), false);
		destroy_wave(&wave);
	}
	return container;
}