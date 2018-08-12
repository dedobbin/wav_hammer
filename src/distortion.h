#ifndef HAMMING_H_
#define HAMMING_H_

#include "raw_wave.h"

void hamming_distortion(Raw_wave * wave, long amount);
void hamming_pointless_distortion(Raw_wave * wave);
void hamming_distortion2(Raw_wave * dst);
void hamming_distortion3(Raw_wave * dst);
void distortion(Raw_wave * wave, long amount);
void hamming_kapot(Raw_wave * dst);

#endif
