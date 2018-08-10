#include <stdio.h>
#ifdef __linux__ 
#include <unistd.h>
#elif _WIN32
#endif
#include <string.h>
#include "raw_wave.h"
#include "distortion.h"
#include "datatypes.h"
#include "merge_waves.h"

#define MAX_CONFIG_FILE_SIZE 255
#define EXIT_SUCCESS 0
#define ERROR_COULD_NOT_READ_FILE 1
#define ERROR_NOT_ENOUGH_MEMORY 2
#define ERROR_INVALID_CMD_ARGUMENTS 3
#define ERROR_INVALID_CONFIG_FILE 4

typedef struct Configs {
	char * input_folder;
	char * output_file;
	int min_src_samples;
	int max_src_samples;
	int min_src_offset;
	int max_src_offset;
} Configs;

int parse_config_file(Configs * configs, char * path)
{
	configs->input_folder = NULL;
	configs->output_file = NULL;
	FILE * f;
	f = fopen("../../config.ini", "rb");
	if (!f) return ERROR_COULD_NOT_READ_FILE;
	fseek(f, 0L, SEEK_END);
	long filesize = ftell(f);
	fseek(f, 0L, SEEK_SET);
	uint8_t * file_buffer = malloc(filesize + 1);
	if (!file_buffer) return ERROR_NOT_ENOUGH_MEMORY;
	fread(file_buffer, 1, filesize, f);
	file_buffer[filesize] = '\0';
	int i = 0;
	char c;
	char key_buffer[100];
	do {

		//Read until a complete config 'key' is in buffer
		int offset = i;
		do {
			c = file_buffer[i];
			key_buffer[i - offset] = c;
			i++;
		} while (c != 61 && c != '\0'); //61 is ascii code for '='
		key_buffer[i - offset - 1] = '\0'; //= is also memcpy'd, just overwrite it

		char value_buffer[100];

		//Read until a complete config 'value' is in buffer
		offset = i;
		do {
			c = file_buffer[i];
			value_buffer[i - offset] = c;
			i++;
		} while (c != '\n' && c != '\0');

#ifdef _WIN32
		if (c == '\n') {
			value_buffer[i - offset - 2] = '\0';//\n\r is also memcpy'd, just overwrite it
		}
		else {
			value_buffer[i - offset - 1] = '\0';//\0 is also memcpy'd, just overwrite it
		}
#else
		value_buffer[i - offset - 1] = '\0';//\n is also memcpy'd, just overwrite it
#endif

		//Check what config rule we are dealing with
		if (strcmp(value_buffer, "")==0) {
			if (configs->input_folder) free(configs->input_folder);
			if (configs->output_file) free(configs->output_file);
			return ERROR_INVALID_CONFIG_FILE;
		}
		else if (strcmp(key_buffer, "input_folder") == 0) {
			configs->input_folder = malloc(strlen(value_buffer) + 1);
			strcpy(configs->input_folder, value_buffer);
		}
		else if (strcmp(key_buffer, "output_file") == 0) {
			configs->output_file = malloc(strlen(value_buffer) + 1);
			strcpy(configs->output_file, value_buffer);
		}
		else if (strcmp(key_buffer, "min_src_samples") == 0) {
			configs->min_src_samples = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "max_src_samples") == 0) {

			configs->max_src_samples = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "min_src_offset") == 0) {
			offset = i;
			configs->min_src_offset = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "max_src_offset") == 0) {
			configs->max_src_offset = atoi(value_buffer);
		}
	} while (c != '\0');

	return 0;
}

int main(int argc, char* argv[])
{

	/**
	commandline arguments: 
	1: input folder
	2: output file path
	3: minimal amount or random samples from input file
	4: maxium amount or random samples from input file
	5: minimal offset from input file
	6: maxium offset from input file

	OR 

	1: config file
	**/
	
	if (argc == 7) {
		printf("Merging waves..\n");
		Raw_wave * wave = merge_waves(argv[1], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
		printf("Saving wave file to %s..\n", argv[2]);
		write_wave(wave, argv[2]);
		destroy_wave(&wave);
		printf("Edn");
		getchar();
		return EXIT_SUCCESS;
	} else if (argc == 2) {
		Configs * configs = malloc(sizeof(Configs));
		int result = parse_config_file(configs, argv[1]);
		if (result != 0) return result;
		printf("Merging waves..\n");
		Raw_wave * wave = merge_waves(configs->input_folder, configs->min_src_samples, configs->max_src_samples, configs->min_src_offset, configs->max_src_offset);
		printf("Saving wave file to %s..\n", configs->output_file);

		write_wave(wave, configs->output_file);
		destroy_wave(&wave);
		if (configs->input_folder) free(configs->input_folder);
		if (configs->output_file) free(configs->output_file);
		free(configs);
		printf("Edn");
		getchar();
		return EXIT_SUCCESS;
	} else {
		printf("No arguments given..\n");
		printf("Commandline arguments:\n");
		printf("1:\tinput folder\n2: \toutput file path\n3: \tminimal amount or random samples from input file\n4: \tmaxium amount or random samples from input file\n5: \tminimal offset from input file\n6: \tmaxium offset from input file");
		printf("\nOR\n\n");
		printf("1: path to config file\n");
		return ERROR_INVALID_CMD_ARGUMENTS;
	}
}

