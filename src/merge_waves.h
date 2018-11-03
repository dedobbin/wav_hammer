#ifndef FILEPICKER_H_
#define FILEPICKER_H_

#include "raw_wave.h"
#include <stdio.h>

#define MAX_INPUT_FILES 10000

Raw_wave * glue_waves_random_autovalues(char * path);
/* times means how many times files from folder will be read to put in dstList[] */
Raw_wave * glue_waves_random(char * path, int amount_min, int amount_max, int offset_min, int offset_max, int perc_random_skip, int times);
void merge_waves(Raw_wave * wave_one, Raw_wave * wave_two);

#endif
