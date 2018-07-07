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

void insert_samples_llist(Raw_wave * wave, int offset, Linked_list * list, bool overwrite)
{ 
  if (overwrite) {
	  if (offset + list->size <= num_samples(wave)) {
		  int i = 0;
		  Node * node = list->tail;
		  while (node) {
			  set_sample(wave, i++ + offset, node->data);
			  node = node->next;
		  }
	  }
  }
  else {
	  printf("zoinks, insert_samples without overwrite not implemented yet");
	  //TODO
  }
  return;
}
