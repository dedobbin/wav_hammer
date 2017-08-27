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
    strcpy(ifile, "audio/coconut.wav");
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
  printf("\noriginal wave:\n");
  print_wave(w);

  Linked_list * list = extract_samples(w);
  insert_samples(&w, list);
  llist_destroy(&list);

  printf("\nnew wave: \n");
  print_wave(w);
  write_wave(w, ofile);
  destroy_wave(&w); 
  return 0;
}

