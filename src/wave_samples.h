#ifndef WAVE_SAMPLES_H
#define WAVE_SAMPLES_H

#include "raw_wave.h"
#include "datatypes.h"

long get_sample(Raw_wave * wave, int nSample);
void set_sample(Raw_wave * wave, int nSample, long value);
void insert_samples(Raw_wave * dst, Raw_wave * src, long src_amount, long src_offset, long dst_offset, bool overwrite);
void extract_samples_llist(Linked_list * result, Raw_wave * wave, int num);

#endif

