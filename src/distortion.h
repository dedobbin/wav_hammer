#ifndef HAMMING_H_
#define HAMMING_H_

#include "raw_wave.h"

void hamming_distortion(Raw_wave * wave, long amount);
void hamming_pointless_distortion(Raw_wave * wave);
void distortion(Raw_wave * wave, long amount);

#endif
