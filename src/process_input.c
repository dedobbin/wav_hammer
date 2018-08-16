#include <stdio.h>
#ifdef __linux__ 
#include <unistd.h>
#elif _WIN32
#endif
#include <string.h>
#include "process_input.h"
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
		if (strcmp(key_buffer, "") == 0 || strcmp(value_buffer, "") == 0) {
			if (configs->input_folder) free(configs->input_folder);
			if (configs->output_file) free(configs->output_file);
			return ERROR_INVALID_CONFIG_FILE;
		}
		//Check what config rule we are dealing with
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

int process_commandline_arguments(int argc, char * argv[])
{
	if (argv == 7){
		printf("Merging waves..\n");
		Raw_wave * wave = merge_waves_random(argv[1], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
		printf("Saving wave file to %s..\n", argv[2]);
		write_wave(wave, argv[2]);
		destroy_wave(&wave);
		printf("Edn");
		return EXIT_SUCCESS;
	}
	 else if (argc == 2) {
		 int i = 0;
		 Configs * configs = malloc(sizeof(Configs));
		 int result = parse_config_file(configs, argv[1]);
		 if (result != 0) return result;
		 printf("Merging waves..\n");
		 Raw_wave * wave = merge_waves_random(configs->input_folder, configs->min_src_samples, configs->max_src_samples, configs->min_src_offset, configs->max_src_offset);
		 printf("Saving wave file to %s..\n", configs->output_file);

		 write_wave(wave, configs->output_file);
		 destroy_wave(&wave);
		 if (configs->input_folder) free(configs->input_folder);
		 if (configs->output_file) free(configs->output_file);
		 free(configs);
		 printf("Edn");
		 return EXIT_SUCCESS;
	 }
}

int process_interactive_input()
{
	printf("No arguments given..\n");
	printf("Commandline arguments:\n");
	printf("1:\tinput folder\n2: \toutput file path\n3: \tminimal amount or random samples from input file\n4: \tmaxium amount or random samples from input file\n5: \tminimal offset from input file\n6: \tmaxium offset from input file");
	printf("\n\nOR\n\n");
	printf("1: path to config file\n");
	printf("\nType i and press enter for interactive mode, or just press enter to exit\n");
	char result = getchar();
	if (result != 'i')
		return EXIT_SUCCESS;
	else {
#define MAX_LEN_INPUT_STRING 255
		printf("Interactive mode\n");
		char buffer[MAX_LEN_INPUT_STRING];

		Raw_wave * result = create_header();
		Raw_wave * wave_two = create_header();
		
		do {
			do {
				printf("Take samples from what file?\n");
				scanf("%s", buffer);
			} while (load_wave(&wave_two, buffer) < 0);

			printf("Offset from source file? (non-number for no offset)\n");
			scanf("%s", buffer);
			int src_offset = atoi(buffer);

			int n_samples = 0;
			printf("Take all samples? y for yes, number for amount\n");
			scanf("%s", buffer);
			if (buffer[0] >= 'A' && buffer[0] <= 'z') {
				n_samples = num_samples(wave_two);
			} else {
				n_samples = atoi(buffer);
			}

			//Place segment in new container
			Raw_wave * wave_two_segment = create_header();
			insert_samples(wave_two_segment, wave_two, n_samples, src_offset, 0, false);

			char arg_buffer[100];
			printf("Mutate audio?\n0:\tNo\n1:\tdistortion(n)\n2:\thamming_kapot()\n3:\thamming_distortion(n)\n4:\thamming_punch_distortion()\n5:\thamming_pointless_distortion()\n6:\thamming_pointless_distortion2()\n7:\thamming_pointless_distortion3()\n");
			scanf("%s", buffer);

			switch (atoi(buffer)) {
				case 0:
					break;
				case 1:
					printf("Argument?\n");
					scanf("%s", buffer);
					distortion_multiply(wave_two_segment, atoi(buffer));
					break;
				case 2:
					hamming_kapot(wave_two_segment);
					break;
				case 3:
					printf("Argument?\n");
					scanf("%s", buffer);
					distortion_multiply(wave_two_segment, atoi(buffer));
					break;
				case 4:
					hamming_punch_distortion(wave_two_segment);
					break;
				case 5:
					hamming_pointless_distortion(wave_two_segment);
					break;
				case 6:
					hamming_pointless_distortion2(wave_two_segment);
					break;
				case 7:
					hamming_pointless_distortion3(wave_two_segment);
					break;
				default:
					break;
			}

			int insert_point = 0;
			if (num_samples(result) > 0) {
				printf("Append to end of source file? y for yes, number for insert point\n");
				scanf("%s", buffer);
				if (buffer[0] >= 'A' && buffer[0] <= 'z') {
					insert_point = num_samples(result);
				} else {
					insert_point = atoi(buffer);
				}

			}
			//void insert_samples(Raw_wave * dst, Raw_wave * src, long src_amount, long src_offset, long dst_offset, bool overwrite);
			insert_samples(result, wave_two_segment, num_samples(wave_two_segment), 0, insert_point, false);

			printf("Add another file?\n");
			scanf("%s", buffer);
		} while (buffer[0] == 'y');

		do {
			printf("Output file?\n");
			scanf("%s", buffer);
		} while (write_wave(result, buffer) < 0);
	}
}