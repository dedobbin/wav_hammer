#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "wave_samples.h"

long get_sample(Raw_wave * wave, int nSample)
{
  long result = 0;
  if (nSample <= num_samples(wave)){
    assert(sizeof(long) >= 4);
    int bytesPerSample = bits_per_sample(wave) / 8;
    char * ptr = wave->data_chunk->audiodata + nSample * bytesPerSample;
    memcpy(&result, ptr, bytesPerSample);
  }
  return result;
}

void set_sample(Raw_wave * wave, int nSample, long value)
{
  if (nSample <= num_samples(wave)){
	  int bytesPerSample = bits_per_sample(wave) / 8;
    memcpy(wave->data_chunk->audiodata + nSample * bytesPerSample, &value, bytesPerSample);
  }
}

void extract_samples_llist(Linked_list * result, Raw_wave * wave, int num)
{
  if (!result || !wave)
    return -1;
  
  int limit = 0;
  if (num >= num_samples(wave))
    limit = num_samples(wave);
  else {
    limit = num;
  }
  int i;
  for (i = 0; i < limit; ++i){
    llist_push(&result, get_sample(wave, i));
  }
}

// Core logic of insert_samples(), called by insert_samples_VAR
void insert_samples_BASE(Raw_wave * dst, Raw_wave * src, long src_amount, long src_offset, long dst_offset, bool overwrite)
{
    if (!overwrite) {
		//conversion from n_samples to n_bytes: datasize = bytes per samples * number of channels * n samples
		int bytesPerSample = bits_per_sample(src) / 8;
		int srcOffsetInSamples = bytesPerSample * src_offset;

		if (num_channels(src) != num_channels(dst)) {
			printf("insert_samples: Trying to insert %d-channel data to %d-channel data, aborting\n", num_channels(src), num_channels(dst));
			return -2;
		}

		if (src_offset >= num_samples(src))
			return -3;

		if (src_amount + src_offset > num_samples(src))
			src_amount = num_samples(src) - src_offset;

		if (dst_offset > num_samples(dst))
			dst_offset = num_samples(dst);

		//get samples of dst wave before dst_offset, they should be left intact
		long dataChunkOneSize = bytesPerSample * dst_offset;
		long dataChunkTwoSize = bytesPerSample * src_amount;
		long dataChunkThreeSize = bytesPerSample * (num_samples(dst) - dst_offset);
		long combinedDataChunksize = dataChunkOneSize + dataChunkTwoSize + dataChunkThreeSize;
		uint8_t *  combinedDataChunk = malloc(combinedDataChunksize);

		//samples from src before dst_offset should be left intact
        memcpy(combinedDataChunk, dst->data_chunk->audiodata, dataChunkOneSize);
		//get amount of samples from src to insert
		memcpy(combinedDataChunk + dataChunkOneSize, src->data_chunk->audiodata + srcOffsetInSamples, dataChunkTwoSize);
		//get tail part of original dst wave, they should stay intact
		long offsetInBytes = dst->data_chunk->audiodata + bits_per_sample(dst) / 8 * dst_offset;
		memcpy(combinedDataChunk + dataChunkOneSize + dataChunkTwoSize, offsetInBytes, dataChunkThreeSize);

        if (dst->data_chunk->audiodata)
			free(dst->data_chunk->audiodata);
        dst->data_chunk->audiodata = combinedDataChunk;

		set_datasize(dst, combinedDataChunksize);

    } else {
        if (src_amount + dst_offset > num_samples(dst)) {
            printf("insert_samples: too much samples to insert in destination wave, aborting\n");
            return -1;
        }
        long i;
        for (i = 0; i < src_amount; i++) {
            set_sample(dst, i + dst_offset, get_sample(src, i));
        }
        return 1;
    }
}

// Called by 'calling' insert_samples(), which is #defind in wave_samples.h
// Checks if input params are set, if not use defaults
void insert_samples_VAR(insert_samples_args in)
{
	if (in.src == NULL || in.dst == NULL)
		return;
	
	//Check if values, otherwise use defaults
	in.src_amount = in.src_amount > 0 ? in.src_amount: num_samples(in.src);
	in.src_offset = in.src_offset > 0 ? in.src_offset : 0;
	in. dst_offset = in.dst_offset > 0 ? in.dst_offset : num_samples(in.dst);
	//overwrite should 'default' to 0, which is false, which is good default
	//void insert_samples_BASE(Raw_wave * dst, Raw_wave * src, long src_amount, long src_offset, long dst_offset, bool overwrite)

	insert_samples_BASE(in.dst, in.src, in.src_amount, in.src_offset, in.dst_offset, in.overwrite);
}