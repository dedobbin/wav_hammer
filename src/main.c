#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "raw_wave.h"
#include "hamming.h"
#include "linked_list.h"
#include "datatypes.h"

int main(int argc, char* argv[])
{
  int i;
  
  Linked_list * list = llist_create();
  for (i = 0; i < 3; ++i){
    llist_push(&list, 1, 1);
  }  

  
  Linked_list * list2 = llist_create();
  for (i = 0; i < 2; ++i){
    llist_push(&list2, 2, 2);
  } 
 
  llist_merge(&list, &list2, 1); 

  llist_print(&list);
  llist_print_backwards(&list);

  llist_destroy(&list);
  llist_destroy(&list2);
  
  /**
  char ifile[100];
  char ofile[100];
  if (argc < 3){
    printf("No input/output provided, using default testing values\n");
    strcpy(ifile, "audio/stereo.wav");
    strcpy(ofile, "audio/output.wav");
    //printf("Please provide input and output\n");
    //return 0;
  } 
  else{0
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
  insert_samples_compl(&w, list, num_samples(w), list->size, true);
  llist_destroy(&list);

  printf("\nnew wave: \n");
  print_wave(w);
  write_wave(w, ofile);
  destroy_wave(&w); 
  **/
  return 0;
}

