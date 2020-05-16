#ifndef WAVE_SAMPLES_H
#define WAVE_SAMPLES_H

#include "raw_wave.h"
#include "datatypes.h"

typedef struct {
	Raw_wave * dst;
	Raw_wave * src;
	long src_amount;
	long src_offset;
	long dst_offset;
	bool overwrite;
	bool force_src_sample_rate;
} insert_samples_args;


//Included so inser_samples marco does not give warning, could also perhaps use directly
void insert_samples_VAR(insert_samples_args);
//When user 'calls' insert_samples(), params are used to populate struct, which is passed to insert_samples_VAR
#define insert_samples(...)insert_samples_VAR((insert_samples_args){__VA_ARGS__})

long get_sample(Raw_wave * wave, int nSample);
void set_sample(Raw_wave * wave, int nSample, long value);
void extract_samples_llist(Linked_list * result, Raw_wave * wave, int num);
void remove_channel(Raw_wave * input, int channel);

#endif

