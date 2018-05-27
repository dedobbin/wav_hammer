#ifndef WAVE_SAMPLES_H
#define WAVE_SAMPLES_H

#include "linked_list.h"
#include "raw_wave.h"
#include "datatypes.h"

long get_sample(Raw_wave * wave, int nSample);
void set_sample(Raw_wave * wave, int nSample, long value);
void extract_samples(Linked_list * result, Raw_wave * wave);
void insert_samples(Raw_wave ** dst, Linked_list * src);
//Appends n samples from src (starting at index) to dst
void insert_samples_compl(Raw_wave ** dst, Linked_list * src, int index, int n, bool overwrite);

#endif
