#include <math.h>
#include <stdio.h>
#include "distortion.h"

void hamming_distortion(Raw_wave * wave, long amount)
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

void hamming_pointless_distortion(Raw_wave * wave)
{
  int i = 0;
  for (i=0; i < datasize(wave); i++){
    wave->data_chunk->audiodata[i]*=2;
  }
}

void hamming_distortion2(Raw_wave * dst)
{

	int i = 0;
	for (i = 0; i < datasize(dst); i++) {
		dst->data_chunk->audiodata[i] *= 2;
	}
}

void hamming_distortion3(Raw_wave * dst)
{

	int i = 0;
	for (i = 0; i < datasize(dst) - 1; i++) {
		dst->data_chunk->audiodata[i] |= dst->data_chunk->audiodata[i + 1];
	}
}

void distortion(Raw_wave * wave, long amount)
{

	if (bits_per_sample(wave) == 16) {
		//-32768 to 32767

		const MAX = 32767;
		const MIN = -32768;

		int i;
		for (i = 0; i < num_samples(wave); i++) {
			int16_t sample = get_sample(wave, i);
			//printf("before: %d ||\t", sample);
			if (sample > 0) {
				if (sample + amount < sample)
					set_sample(wave, i, MAX);
				else
					set_sample(wave, i, sample + amount);
			}
			else if (sample < 0) {
				if (sample - amount > sample)
					set_sample(wave, i, MIN);
				else
					set_sample(wave, i, sample - amount);
			}
			//printf("after: %d\n", get_sample(wave, i));
		}
	} 
	printf("done hamming\n");
}

void hamming_kapot(Raw_wave * dst)
{
	int i = 0;
	for (i = 0; i < datasize(dst) ; i++) {
		dst->data_chunk->audiodata[i] |= dst->data_chunk->audiodata[random(0, i)];
	}
}
