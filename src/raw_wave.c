#ifdef __linux__ 
#include <unistd.h>
#elif _WIN32
#include <io.h>
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
    fprintf(stderr, "load_wave: Could not open file '%s' for reading\n", path);
    return -2;
  }
  #elif _WIN32
  if (!f || (_access(path, 0) == -1)){
	  fprintf(stderr, "load_wave: Could not open file '%s' for reading\n", path);
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
  (*wave)->riff_chunk = NULL;
  (*wave)->fmt_chunk = NULL;
  (*wave)->data_chunk = NULL;
  (*wave)->info_chunk = NULL;

  //teh RIFF chunk 
  (*wave)->riff_chunk = malloc( RIFF_CHUNK_SIZE);
  memcpy((*wave)->riff_chunk, buffer, RIFF_CHUNK_SIZE); 
  
  //teh fmt chunk 
  (*wave)->fmt_chunk = malloc(FMT_CHUNK_SIZE);
  memcpy((*wave)->fmt_chunk, buffer+RIFF_CHUNK_SIZE, FMT_CHUNK_SIZE);
  
  //Teh data chunk ( = dynamic size)
  (*wave)->data_chunk = malloc(sizeof(Data_chunk));
  (*wave)->data_chunk->raw_header_data = malloc(DATA_CHUNK_HEADER_SIZE);
  memcpy((*wave)->data_chunk->raw_header_data, buffer+DATA_CHUNK_OFFSET, DATA_CHUNK_HEADER_SIZE);
  
  unsigned dsize  = datasize(*wave);
  //Check if there are more bytes in the file, if so, it's an optional INFO chunk
  if (filesize - dsize != DATA_OFFSET){
    printf("load_wave: Datasize != filesize - data offset, should look for INFO chunk at end of file\n");
    memcpy(strbuffer, buffer + DATA_OFFSET + dsize, 4);
    strbuffer[5] = '\0';
    if (strcmp(strbuffer, "LIST") ==0){
      if ( ((*wave)->info_chunk = malloc(sizeof(Info_chunk))) == NULL)
        return -1;
      (*wave)->info_chunk->size = filesize - (DATA_OFFSET + dsize);
      (*wave)->info_chunk->raw_data = malloc((*wave)->info_chunk->size);
      memcpy((*wave)->info_chunk->raw_data, buffer + DATA_OFFSET + dsize, (*wave)->info_chunk->size );
    }
  }
 
  (*wave)->data_chunk->audiodata = malloc(dsize);
  memcpy((*wave)->data_chunk->audiodata, buffer + DATA_OFFSET, dsize); 

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
    fprintf(stderr, "write_wave: Could not open file '%s' for writing\n", path);
    return -1;
  }
#else
  if (!f) {
	  fprintf(stderr, "write_wave: Could not open file '%s' for writing\n", path);
	  return -1;
}
#endif
  
  fwrite(wave->riff_chunk, 1, RIFF_CHUNK_SIZE, f);
  fwrite(wave->fmt_chunk, 1, FMT_CHUNK_SIZE, f);
  fwrite(wave->data_chunk->raw_header_data, 1, DATA_CHUNK_HEADER_SIZE, f);
  fwrite(wave->data_chunk->audiodata, 1, datasize(wave), f);
  if (wave->info_chunk != NULL){
    fwrite(wave->info_chunk->raw_data, 1, wave->info_chunk->size, f);
  }
  fclose(f); 
  return 1;
}

void destroy_wave(Raw_wave ** wave)
{
  if ((*wave)->riff_chunk != NULL){
    if ((*wave)->riff_chunk != NULL){
      free ((*wave)->riff_chunk);
      (*wave)->riff_chunk = NULL;
    }
  }

  if ((*wave)->fmt_chunk != NULL){
    if ((*wave)->fmt_chunk != NULL){
      free ((*wave)->fmt_chunk);
      (*wave)->fmt_chunk = NULL;
    }
  }

  if ((*wave)->data_chunk != NULL){
    if ((*wave)->data_chunk->raw_header_data != NULL){
      free ((*wave)->data_chunk->raw_header_data);
      (*wave)->data_chunk->raw_header_data = NULL;
    }
    if ((*wave)->data_chunk->audiodata != NULL){
      free ((*wave)->data_chunk->audiodata);
      (*wave)->data_chunk->audiodata = NULL;
    }
    free ((*wave)->data_chunk);
  }

  if ((*wave)->info_chunk != NULL){
    if ((*wave)->info_chunk->raw_data != NULL){
      free ((*wave)->info_chunk->raw_data);
      (*wave)->info_chunk->raw_data = NULL;
    }
    free ((*wave)->info_chunk);
    (*wave)->info_chunk->raw_data = NULL;
  }
  free(*wave);
  *wave = NULL;
}

Raw_wave * create_header()
{
	int subchunk1Size = 16;	//16 for pcm
	//ChunkSize is part of riff chunk: 4 + (8 + SubChunk1Size) + (8 + actual datasize)
	//AKA entire file in bytes minus 8 bytes for thetwo fields not included in this count:ChunkID and ChunkSize.
	//So should be 36 for pcm if there is no data.
	int ChunkSize = 36;
	int audioFormat = 1;	//1 for pcm 
	int numChannels = 2;
	int samplerate = 44100;
	int bitsPerSample = 16;
	int blockAlign = numChannels * bitsPerSample / 8;
	int byteRate = samplerate * numChannels * bitsPerSample / 8;

	Raw_wave * wave = NULL;
	if (wave = malloc(sizeof(Raw_wave))) {
		if (wave->riff_chunk = malloc(RIFF_CHUNK_SIZE)) {
			char riffStr[] = "RIFF";
			memcpy(wave->riff_chunk, riffStr, 4);
			memcpy(wave->riff_chunk + 4, &ChunkSize, 4);
			char waveStr[] = "WAVE";
			memcpy(wave->riff_chunk + 8, waveStr, 4);
		}
		if (wave->fmt_chunk = malloc(FMT_CHUNK_SIZE)) {
			char fmtStr[] = "fmt ";
			memcpy(wave->fmt_chunk, fmtStr, 4);
			memcpy(wave->fmt_chunk + 4, &subchunk1Size, 4);
			memcpy(wave->fmt_chunk + 8, &audioFormat, 2);
			memcpy(wave->fmt_chunk + 10, &numChannels, 2);
			memcpy(wave->fmt_chunk + 12, &samplerate, 4);
			memcpy(wave->fmt_chunk + 16, &byteRate, 4);
			memcpy(wave->fmt_chunk + 20, &blockAlign, 2);
			memcpy(wave->fmt_chunk + 22, &bitsPerSample, 2);
		}
		if (wave->data_chunk = malloc(sizeof(Data_chunk))) {
			if (wave->data_chunk->raw_header_data = malloc(DATA_CHUNK_HEADER_SIZE)) {
				char dataStr[] = "data";
				memcpy(wave->data_chunk->raw_header_data, dataStr, 4);
				int dataSize = 0;
				memcpy(wave->data_chunk->raw_header_data + 4, &dataSize, 4);
			}
		}
		wave->data_chunk->audiodata = NULL;
		wave->info_chunk = NULL;
	}
	return wave;
}

void print_wave(Raw_wave * wave, bool verbose)
{

	int i;
	if (verbose) {
		printf("riff data: ");
		for (i = 0; i < RIFF_CHUNK_SIZE; ++i) {
			printf("%X ", wave->riff_chunk[i] & 0xFF);
		}
		printf("\n");

		printf("fmt data: ");
		for (i = 0; i < FMT_CHUNK_SIZE; ++i) {
			printf("%X ", wave->fmt_chunk[i] & 0xFF);
		}
		printf("\ndata header: ");
		for (i = 0; i < DATA_CHUNK_HEADER_SIZE; ++i) {
			printf("%X ", wave->data_chunk->raw_header_data[i] & 0xFF);
		}
		if (wave->info_chunk != NULL && wave->info_chunk->raw_data != NULL && wave->info_chunk->size > 0) {
			printf("\ninfo chunk size %d\nlast four bytes: ", wave->info_chunk->size);
			for (i = wave->info_chunk->size - 4; i < wave->info_chunk->size; ++i) {
				printf("%x ", wave->info_chunk->raw_data[i]);
			}
		}
		printf("\n");

		printf("format: %d\n", audio_format(wave));
		printf("numChannels: %d\n", num_channels(wave));
		printf("samplerate: %d\n", samplerate(wave));
		printf("block_align: %d\n", block_align(wave));
		printf("chunk_size: %d\n", chunk_size(wave));
	}
	printf("datasize: %dB\n", datasize(wave));
	printf("byterate: %d\n", byterate(wave));
	printf("bits per sample: %d\n", bits_per_sample(wave));
	printf("number of samples: %d\n", num_samples(wave));
	printf("First 4 samples: %08lx %08lx %08lx %08lx\n", 
	get_sample(wave, 0), get_sample(wave, 1), get_sample(wave, 2), get_sample(wave, 3));
}

unsigned chunk_size(const Raw_wave * wave)
{
	//4 byte value from riff_chunk, offset 4
	uint32_t result = 0;
	int i;
	for (i = 0; i < 4; i++) {
		int value = *(wave->riff_chunk + 4 + i);
		result |= value << 8 * i;
	}
	return result;
}

unsigned audio_format(const Raw_wave * wave)
{
	//2 byte value from fmt_chunk, offset 8
	uint16_t result = 0;
	int i;
	for (i = 0; i < 2; i++) {
		int value = *(wave->fmt_chunk + 8 + i);
		result |= value << 8 * i;
	}
	return result;
}


unsigned num_channels(const Raw_wave * const wave)
{
	//2 byte value from fmt_chunk, offset 10
	uint16_t result = 0;
	int i;
	for (i = 0; i < 2; i++) {
		int value = *(wave->fmt_chunk + 10 + i);
		result |= value << 8 * i;
	}
	return result;
}

unsigned samplerate(const Raw_wave * const wave)
{
	//4 byte value from fmt_chunk, offset 12
	uint32_t result = 0;
	int i;
	for (i = 0; i < 4; i++) {
		int value = *(wave->fmt_chunk + 12 + i);
		result |= value << 8 * i;
	}
	return result;
}

unsigned byterate(const Raw_wave * const wave)
{
	//4 byte value from fmt_chunk, offset 16
	uint32_t result = 0;
	int i;
	for (i = 0; i < 4; i++) {
		int value = *(wave->fmt_chunk + 16 + i);
		result |= value << 8 * i;
	}
	return result;
}

unsigned block_align(const Raw_wave * const wave)
{
	//2 byte value from fmt_chunk, offset 20
	uint16_t result = 0;
	int i;
	for (i = 0; i < 2; i++) {
		int value = *(wave->fmt_chunk + 20 + i);
		result |= value << 8 * i;
	}
	return result;
}

unsigned bits_per_sample(const Raw_wave * const wave)
{
	//2 byte value from fmt_chunk, offset 22
	uint16_t result = 0;
	int i;
	for (i = 0; i < 2; i++) {
		int value = *(wave->fmt_chunk + 22 + i);
		result |= value << 8 * i;
	}
	return result;
}

unsigned datasize(const Raw_wave * const wave)
{
	//4 byte value from data_chunk, offset 4
	uint32_t result = 0;
	int i;
	for (i = 0; i < 4; i++) {
		int value = *(wave->data_chunk->raw_header_data + 4 + i);
		result |= value << 8 * i;
	}
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
	memcpy(wave->fmt_chunk + 10, &numChannels, 2);
}

void set_block_align(Raw_wave * wave, int blockAlign)
{
	memcpy(wave->fmt_chunk + 20, &blockAlign, 2);
}

void set_datasize(Raw_wave * wave, int dataSize)
{
	memcpy(wave->data_chunk->raw_header_data + 4, &dataSize, 4);
}

void set_info_chunk(Raw_wave * const wave, const uint8_t * const new_info_chunk_data, int new_info_chunk_size)
{
	if (!wave->info_chunk) {
		wave->info_chunk = malloc(sizeof(wave->info_chunk));
	}
	else if (wave->info_chunk->raw_data) {
		free(wave->info_chunk->raw_data);
	}

	wave->info_chunk->raw_data = malloc(new_info_chunk_size);
	memcpy(wave->info_chunk->raw_data, new_info_chunk_data, new_info_chunk_size);
	wave->info_chunk->size = new_info_chunk_size;
}
