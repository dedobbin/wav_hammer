#ifndef WAVE_SAMPLES_H
#define WAVE_SAMPLES_H

#include "linked_list.h"
#include "raw_wave.h"
#include "datatypes.h"

long get_sample(Raw_wave * wave, int nSample);
void set_sample(Raw_wave * wave, int nSample, long value);
void insert_samples(Raw_wave * dst, Raw_wave * src, long amount, long dst_offset);

void extract_samples_llist(Linked_list * result, Raw_wave * wave, int num);

#endif

