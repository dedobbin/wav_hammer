#ifdef __linux__ 
#include <unistd.h>
#elif _WIN32
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "raw_wave.h"


static const DATA_CHUNK_OFFSET = 36;
static const DATA_OFFSET = 44;

//TODO: should just return ptr?
int load_wave(Raw_wave ** wave, const char* const path)
{
  FILE * f;
  f = fopen(path, "rb");
  #ifdef __linux__ 
  if (!f || access(path, R_OK)){
    fprintf(stderr, "Could not open file '%s' for reading\n", path);
    return -2;
  }

  #endif
  
  fseek(f, 0L, SEEK_END);
  long filesize = ftell(f);
  fseek(f, 0L, SEEK_SET);
  uint8_t * buffer = malloc(filesize); 
  //Read raw data into buffer
  fread(buffer, 1, filesize, f);
  
  //Check if is RIFF
  char strbuffer[6];
  memcpy(strbuffer, buffer, 4);
  strbuffer[4] = '\0';
  if (strcmp(strbuffer, "RIFF")!=0){
     fprintf(stderr, "No valid RIFF\n");
     return -3;
  }

  //Check if is wave
  memcpy(strbuffer, buffer+8, 4);
  strbuffer[5] = '\0';
  if (strcmp(strbuffer, "WAVE")!=0){
     fprintf(stderr, "No valid wave\n");
     return -3;
  }
  //Valid wave file, so it's time to start getting data
  if ( ((*wave) = malloc(sizeof(Raw_wave))) == NULL )
    return -1;
  (*wave)->riff = NULL;
  (*wave)->fmt = NULL;
  (*wave)->data = NULL;
  (*wave)->info = NULL;

  //teh RIFF chunk 
  if ( ((*wave)->riff = malloc(sizeof(RIFF_chunk))) == NULL )
    return -1;
  (*wave)->riff->raw_data = malloc( RIFF_CHUNK_SIZE);
  memcpy((*wave)->riff->raw_data, buffer, RIFF_CHUNK_SIZE); 
  
  //teh fmt chunk 
  (*wave)->fmt = malloc(sizeof(Fmt_chunk));
  if ( ((*wave)->fmt->raw_data = malloc(FMT_CHUNK_SIZE)) == NULL){
    return -1;
  }
  memcpy((*wave)->fmt->raw_data, buffer+RIFF_CHUNK_SIZE, FMT_CHUNK_SIZE); 
  
  //Teh data chunk ( = dynamic size)
  (*wave)->data = malloc(sizeof(Data_chunk));


  (*wave)->data->raw_header_data = malloc(DATA_CHUNK_HEADER_SIZE);
  memcpy((*wave)->data->raw_header_data, buffer+DATA_CHUNK_OFFSET, DATA_CHUNK_HEADER_SIZE);
  
  unsigned dsize  = datasize(*wave);
  //Check if there are more bytes in the file, if so, it's an optional INFO chunk
  if (filesize - dsize != DATA_OFFSET){
    printf("Datasize != filesize - data offset, should look for INFO chunk at end of file\n");
    memcpy(strbuffer, buffer + DATA_OFFSET + dsize, 4);
    strbuffer[5] = '\0';
    if (strcmp(strbuffer, "LIST") ==0){
      if ( ((*wave)->info = malloc(sizeof(Info_chunk))) == NULL)
        return -1;
      (*wave)->info->size = filesize - (DATA_OFFSET + dsize);
      (*wave)->info->raw_data = malloc((*wave)->info->size);
      memcpy((*wave)->info->raw_data, buffer + DATA_OFFSET + dsize, (*wave)->info->size );
    }
  }
 
  (*wave)->data->audiodata = malloc(dsize);
  memcpy((*wave)->data->audiodata, buffer + DATA_OFFSET, dsize); 

  //Clean stuff 
  free(buffer);
  fclose(f);

  return 1;
}

int write_wave(Raw_wave * wave, const char * const path)
{
  FILE * f;
  f = fopen(path, "wb");
  #ifdef __linux__ 
  if (!f || access (path, W_OK)){
    fprintf(stderr, "Could not open file '%s' for writing\n", path);
    return -1;
  }
  #endif
  fwrite(wave->riff->raw_data, 1, RIFF_CHUNK_SIZE, f);
  fwrite(wave->fmt->raw_data, 1, FMT_CHUNK_SIZE, f);
  fwrite(wave->data->raw_header_data, 1, DATA_CHUNK_HEADER_SIZE, f);
  fwrite(wave->data->audiodata, 1, datasize(wave), f);
  if (wave->info != NULL){
    fwrite(wave->info->raw_data, 1, wave->info->size, f);
  }
  fclose(f); 
  return 1;
}

void destroy_wave(Raw_wave ** wave)
{
  if ((*wave)->riff != NULL){
    if ((*wave)->riff->raw_data != NULL){
      free ((*wave)->riff->raw_data);
      (*wave)->riff->raw_data = NULL;
    }
    free ((*wave)->riff);
    (*wave)->riff = NULL;
  }

  if ((*wave)->fmt != NULL){
    if ((*wave)->fmt->raw_data != NULL){
      free ((*wave)->fmt->raw_data);
      (*wave)->fmt->raw_data = NULL;
    }
    free ((*wave)->fmt);
    (*wave)->fmt = NULL;
  }

  if ((*wave)->data != NULL){
    if ((*wave)->data->raw_header_data != NULL){
      free ((*wave)->data->raw_header_data);
      (*wave)->data->raw_header_data = NULL;
    }
    if ((*wave)->data->audiodata != NULL){
      free ((*wave)->data->audiodata);
      (*wave)->data->audiodata = NULL;
    }
    free ((*wave)->fmt);
  }

  if ((*wave)->info != NULL){
    if ((*wave)->info->raw_data != NULL){
      free ((*wave)->info->raw_data);
      (*wave)->info->raw_data = NULL;
    }
    free ((*wave)->info);
    (*wave)->info->raw_data = NULL;
  }
  free(*wave);
  *wave = NULL;
}

void print_wave(Raw_wave * wave)
{
  int i;
  printf("riff data: ");
  for (i = 0; i <RIFF_CHUNK_SIZE;++i){
      printf("%X ", wave->riff->raw_data[i] & 0xFF);
  }
  printf("\n");

  printf("fmt data: ");
  for (i = 0; i <FMT_CHUNK_SIZE;++i){
      printf("%X ", wave->fmt->raw_data[i] & 0xFF);
  }
  printf("\ndata header: ");
  for (i = 0; i <DATA_CHUNK_HEADER_SIZE;++i){
      printf("%X ", wave->data->raw_header_data[i] & 0xFF);
  }
  if (wave->info != NULL){
    printf("\ninfo chunk size %d\nlast four bytes: ", wave->info->size);
    for (i = wave->info->size-4; i < wave->info->size; ++i){
      printf("%x ", wave->info->raw_data[i]);
    }
  }
  printf("\n");

  printf("datasize: %dB\n", datasize(wave));
  printf("format: %d\n", audio_format(wave));
  printf("numChannels: %d\n", num_channels(wave));
  printf("samplerate: %d\n", samplerate(wave));
  printf("block_align: %d\n", block_align(wave));
  printf("byterate: %d\n", byterate(wave));
  printf("bits per sample: %d\n", bits_per_sample(wave));
  printf("number of samples: %d\n", num_samples(wave));
  printf("First 2 samples: %08lx %08lx \n", 
    get_sample(wave, 0), get_sample(wave, 1));
}

unsigned audio_format(const Raw_wave * wave)
{
  unsigned result = 0;
  memcpy(&result, wave->fmt->raw_data+8, 2);
  return result;
}


unsigned num_channels(const Raw_wave * const wave)
{
  unsigned result = 0;
  memcpy(&result, wave->fmt->raw_data+10, 2);
  return result;
}

unsigned samplerate(const Raw_wave * const wave)
{
  unsigned result = 0;
  memcpy(&result, wave->fmt->raw_data+12, 4);
  return result;
}

unsigned byterate(const Raw_wave * const wave)
{
  unsigned result = 0;
  memcpy(&result, wave->fmt->raw_data+16, 4);
  return result;
}

unsigned block_align(const Raw_wave * const wave)
{
  unsigned result = 0;
  memcpy(&result, wave->fmt->raw_data+20, 2);
  return result;
}

unsigned bits_per_sample(const Raw_wave * const wave)
{
  unsigned result = 0;
  memcpy(&result, wave->fmt->raw_data+22, 2);
  return result;
}

unsigned datasize(const Raw_wave * const wave)
{
   unsigned result = 0;
   memcpy(&result, wave->data->raw_header_data + 4, 4);
   return result;
}

unsigned num_samples(const Raw_wave * const wave)
{
  int blockAlign = block_align(wave);
  int size = datasize(wave);
  assert((size % blockAlign) == 0);
  return size / blockAlign;
}

void set_num_channels(Raw_wave * wave, int numChannels)
{
  memcpy(wave->fmt->raw_data+10, &numChannels, 2);
}

void set_block_align(Raw_wave * wave, int blockAlign)
{
  memcpy(wave->fmt->raw_data + 20, &blockAlign, 2);
}

void set_datasize(Raw_wave * wave, int dataSize)
{
  memcpy(wave->data->raw_header_data + 4, &dataSize, 4);
}
