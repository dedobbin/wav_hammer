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

void merge_waves(Raw_wave * dest, Raw_wave * src, long amount, long dst_offset)
{
    bool overwrite = true;
    if (!overwrite) {
        //TODO
        printf("Zoinks, merge_waves with overwrite disabled not implemented yet\n");
    }
    else {

        if (amount + dst_offset > num_samples(dest)) {
            printf("merge_waves: too much samples to insert in destination wave, aborting merge\n");
            return -1;
        }
        long i;
        for (i = 0; i < amount; i++) {
            set_sample(dest, i + dst_offset, get_sample(src, i));
        }
        return 1;
    }
}
