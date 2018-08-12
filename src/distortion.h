#ifndef HAMMING_H_
#define HAMMING_H_

#include "raw_wave.h"

/* actual gain/distortion */
void distortion_multiply(Raw_wave * wave, long amount);
void distortion(Raw_wave * wave, long amount);

/* turns audio into small crackles */
void hamming_kapot(Raw_wave * dst);

/* following functions are general weirdness */
void hamming_distortion(Raw_wave * wave, long amount);
void hamming_punch_distortion(Raw_wave * dst);

/* pointless functions don't take samplesize in consideration, work directly on raw_data */
void hamming_pointless_distortion(Raw_wave * wave);
void hamming_pointless_distortion2(Raw_wave * dst);
void hamming_pointless_distortion3(Raw_wave * dst);

char ** get_distortion_functions();
#endif
