#ifndef FILEPICKER_H_
#define FILEPICKER_H_

#include "raw_wave.h"
#include <stdio.h>

#define MAX_INPUT_FILES 100

Raw_wave * merge_waves_random_autovalues(char * path);
Raw_wave * merge_waves_random(char * path, int amount_min, int amount_max, int offset_min, int offset_max);

#endif