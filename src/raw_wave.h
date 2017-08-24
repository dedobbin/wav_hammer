#ifndef RAW_WAVE_H_
#define RAW_WAVE_H_

#include <stdint.h>
//Bytes 0-11 is riff chunk
static const RIFF_CHUNK_SIZE = 12;
//Bytes 12-35 is fmt chunk 
static const FMT_CHUNK_SIZE = 24;
//Size of data chunk is dynamic, header size is static
static const DATA_CHUNK_HEADER_SIZE = 8;

typedef struct RIFF_chunk{
  uint8_t * raw_data;
}RIFF_chunk;

typedef struct Fmt_chunk{
 uint8_t * raw_data;
}Fmt_chunk;

typedef struct Data_chunk{
  uint8_t * raw_header_data;
  uint8_t * audiodata;
}Data_chunk;

typedef struct Info_chunk{
  uint8_t * raw_data;
  int size;
}Info_chunk;

typedef struct Raw_wave{
  uint8_t* debugPtr;
  RIFF_chunk * riff;
  Fmt_chunk * fmt;
  Data_chunk * data;
  Info_chunk * info;  //Optional
}Raw_wave;

int load_wave(Raw_wave ** wave, const char* const path);
int write_wave(Raw_wave * wave, const char* const path);
void destroy_wave(Raw_wave ** wave);

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
#endif
