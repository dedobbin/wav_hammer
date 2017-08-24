#ifndef WAVE_SAMPLES_H
#define WAVE_SAMPLES_H

#include "linked_list.h"
#include "raw_wave.h"

long get_sample(Raw_wave * wave, int nSample);
void set_sample(Raw_wave * wave, int nSample, long value);
Linked_list * extract_samples(Raw_wave * wave);
insert_sample(Raw_wave ** wave, Linked_list * src);

#endif
