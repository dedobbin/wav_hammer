#ifndef RAW_WAVE_H_
#define RAW_WAVE_H_

#include "datatypes.h"
#include <stdint.h>
//Bytes 0-11 is riff chunk
static const int RIFF_CHUNK_SIZE = 12;
//Bytes 12-35 is fmt chunk 
static const int FMT_CHUNK_SIZE = 24;
//Size of data chunk is dynamic, header size is static
static const int DATA_CHUNK_HEADER_SIZE = 8;


typedef struct Data_chunk{
  uint8_t * raw_header_data;
  uint8_t * audiodata;
}Data_chunk;

typedef struct Info_chunk{
  uint8_t * raw_data;
  int size;
}Info_chunk;

typedef struct Raw_wave{
  uint8_t * riff_chunk;
  uint8_t * fmt_chunk;
  Data_chunk * data_chunk;
  Info_chunk * info_chunk;  //Optional
}Raw_wave;

int load_wave(Raw_wave ** wave, const char* const path);
int write_wave(Raw_wave * wave, const char* const path);
void destroy_wave(Raw_wave ** wave);
Raw_wave * create_header();
void print_wave(Raw_wave * wave, bool verbose);

//Seems to be ignored most of the time
//ChunkSize is part of riff chunk: 4 + (8 + SubChunk1Size) + (8 + actual datasize)
//AKA entire file in bytes minus 8 bytes for thetwo fields not included in this count:ChunkID and ChunkSize.
//So should be 36 for pcm if there is no data.
unsigned chunk_size(const Raw_wave * wave);
unsigned audio_format(const Raw_wave * const wave);
unsigned num_channels(const Raw_wave * const wave);
unsigned samplerate(const Raw_wave * const wave);
unsigned byterate(const Raw_wave * const wave);
unsigned block_align(const Raw_wave * const wave);
unsigned bits_per_sample(const Raw_wave * const wave);
unsigned datasize(const Raw_wave * const wave);
unsigned num_samples(const Raw_wave * const wave);

void set_num_channels(Raw_wave * wave, int numChannels);
void set_block_align(Raw_wave * wave, int blockAlign);
void set_datasize(Raw_wave * wave, int dataSize);
void set_info_chunk(Raw_wave * const wave, const uint8_t * const new_info_chunk_data, int new_info_chunk_size);
#endif
