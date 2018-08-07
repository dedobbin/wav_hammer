#include <math.h>
#include <stdio.h>
#include "distortion.h"

void hamming_gain(Raw_wave * wave, long amount)
{
  int blockAlign = block_align(wave);
  int numSamples = num_samples(wave);
  unsigned long max = pow(2, (blockAlign * 8))-1;
  
  int i;
  for (i = 0; i < numSamples; ++i){
    long sample = get_sample(wave,i);
    set_sample(wave, i, sample + amount);
    long newSample = get_sample(wave, i);
    if (newSample< sample)
      set_sample(wave, i , max);
  }
}

void hamming_pointless_gain(Raw_wave * wave)
{
  int i = 0;
  for (i=0; i < datasize(wave); i++){
    wave->data_chunk->audiodata[i]*=2;
  }
}
