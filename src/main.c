#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "raw_wave.h"
#include "hamming.h"
#include "linked_list.h"

int main(int argc, char* argv[])
{
  int i;
  
  char ifile[100];
  char ofile[100];
  if (argc < 3){
    printf("No input/output provided, using default testing values\n");
    strcpy(ifile, "audio/stereo.wav");
    strcpy(ofile, "audio/output.wav");
    //printf("Please provide input and output\n");
    //return 0;
  } 
  else{
    strcpy(ifile, argv[1]);
    strcpy(ofile, argv[2]);
  }
  
  Raw_wave * w;
  if ( load_wave(&w, ifile) < 0 ){
    printf("Could not initialize Raw_wave struct, exiting\n");
    return 0;
  }
   
  printf("\n");
  printf("main.c: riff data: ");
  for (i = 0; i <RIFF_CHUNK_SIZE;++i){
      printf("%X ", w->riff->raw_data[i] & 0xFF);
  }
  printf("\n");
  
  printf("main.c: fmt data: ");
  for (i = 0; i <FMT_CHUNK_SIZE;++i){
      printf("%X ", w->fmt->raw_data[i] & 0xFF);
  }
  printf("\nmain.c: data header: ");
  for (i = 0; i <DATA_CHUNK_HEADER_SIZE;++i){
      printf("%X ", w->data->raw_header_data[i] & 0xFF);
  }
  if (w->info != NULL){
    printf("\nwave:c Info chunk of size %d\n  last four bytes:", w->info->size);
    for (i = w->info->size-4; i < w->info->size; ++i){
      printf("%x ", w->info->raw_data[i]);
    }
   printf("\n"); 
  }

  printf("\n");
  printf("main.c: datasize: %db\n", datasize(w));
  printf("main.c: format: %d\n", audio_format(w));
  printf("main.c: numChannels: %d\n", num_channels(w));
  printf("main.c: samplerate: %d\n", samplerate(w));
  printf("main.c: block_align: %d\n", block_align(w));
  printf("main.c: byterate: %d\n", byterate(w));
  printf("main.c: bits per sample: %d\n", bits_per_sample(w));
  printf("main.c: number of samples: %d\n", num_samples(w));
  printf("\nmain.c: First 2 samples: %08lx %08lx \n", get_sample(w, 0), get_sample(w, 1));


  Linked_list * list = extract_samples(w);
  insert_samples(&w, list);
  llist_destroy(&list);
  write_wave(w, ofile);
  destroy_wave(&w); 
  return 0;
}

