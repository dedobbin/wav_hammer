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
    bool overwrite = true;
    if (!overwrite) {
        //long newSize = num_samples(src) + num_samples(dst);
        int bytesPerSample = bits_per_sample(src) / 8;
        long newSize = bytesPerSample * num_channels(src) * amount;
        
        //uint8_t * newData = malloc(bytesPerSamples * numChannels * num_samples(src));
        //memcpy(newData, src->data->audiodata, bytesPerSamples * numChannels * num_samples(src));
        uint8_t * newData = malloc(newSize);
        memcpy(newData, src->data->audiodata, newSize);
        set_datasize(src, newSize);

        free(dst->data->audiodata);
        dst->data->audiodata = newData;

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
