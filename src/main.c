#include <stdio.h>
#ifdef __linux__ 
#include <unistd.h>
#elif _WIN32
#endif
#include <string.h>
#include "raw_wave.h"
#include "hamming.h"
#include "linked_list.h"
#include "datatypes.h"


int main(int argc, char* argv[])
{
  char ifile[100];
  char ofile[100];
  if (argc < 3){
    printf("No input/output provided, using default testing values\n");
    strcpy(ifile, "../../audio/input.wav");
    strcpy(ofile, "../../audio/output.wav");
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

  Linked_list * list = llist_create();
  extract_samples_llist(list, w);
  //list->tail->next->data = 0xFF;
  insert_samples_llist(w, 0, list, true);
  llist_destroy(&list);
  
  printf("\nnew wave: \n");
  print_wave(w);
  write_wave(w, ofile);
  destroy_wave(&w); 
  getchar();
  return 0;
}

