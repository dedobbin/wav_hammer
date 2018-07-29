#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "wave_samples.h"

long get_sample(Raw_wave * wave, int nSample)
{
  long result = 0;
  if (nSample <= num_samples(wave)){
    assert(sizeof(long) >= 4);
    int blockAlign = block_align(wave);
    char * ptr = wave->data->audiodata + nSample * blockAlign;
    memcpy(&result, ptr, blockAlign); 
  }
  return result;
}

void set_sample(Raw_wave * wave, int nSample, long value)
{
  if (nSample <= num_samples(wave)){
    int blockAlign = block_align(wave);
    memcpy(wave->data->audiodata + nSample * blockAlign, &value, blockAlign);
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

void merge_waves(Raw_wave * dst, Raw_wave * src, long amount, long dst_offset)
{
    bool overwrite = false;
    if (!overwrite) {
		//datsize = bytes per samples * number of channels * n samples
        int bytesPerSample = bits_per_sample(src) /8;
		int numChannels = num_channels(src);

		//get samples of dst wave before dst_offset, they should be left intact
		long dataChunkOneSize = bytesPerSample * numChannels * dst_offset;
		uint8_t * dataChunkOne = malloc(dataChunkOneSize);
        memcpy(dataChunkOne, dst->data->audiodata, dataChunkOneSize);
        
		//get amount of samples from src to insert
		long dataChunkTwoSize = bytesPerSample * numChannels * amount;
		uint8_t * dataChunkTwo = malloc(dataChunkTwoSize);
		memcpy(dataChunkTwo, src->data->audiodata, dataChunkTwoSize);

		//get tail part of original dst wave, they should stay intact
		long dataChunkThreeSize = bytesPerSample * numChannels * (num_samples(dst) - dst_offset);
		uint8_t * dataChunkThree = malloc(dataChunkThreeSize);
		long offsetInBytes = dst->data->audiodata + bits_per_sample(dst) / 8 * num_channels(dst) * dst_offset;
		memcpy(dataChunkThree, offsetInBytes, dataChunkThreeSize);

		//combine three data chunks
		long combinedDataChunksize = dataChunkOneSize + dataChunkTwoSize + dataChunkThreeSize;
		uint8_t *  combinedDataChunk = malloc(combinedDataChunksize);
		memcpy(combinedDataChunk, dataChunkOne, dataChunkOneSize);
		memcpy(combinedDataChunk + dataChunkOneSize,dataChunkTwo, dataChunkTwoSize);
		memcpy(combinedDataChunk + dataChunkOneSize + dataChunkTwoSize, dataChunkThree, dataChunkThreeSize);

		set_datasize(src, combinedDataChunksize);

		free(dataChunkOne);
		free(dataChunkTwo);
		free(dataChunkThree);

        free(dst->data->audiodata);
        dst->data->audiodata = combinedDataChunk;

    } else {
        if (amount + dst_offset > num_samples(dst)) {
            printf("merge_waves: too much samples to insert in destination wave, aborting merge\n");
            return -1;
        }
        long i;
        for (i = 0; i < amount; i++) {
            set_sample(dst, i + dst_offset, get_sample(src, i));
        }
        return 1;
    }
}
