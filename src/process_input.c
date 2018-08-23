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

#define EXIT_SUCCESS 0
#define ERROR_COULD_NOT_READ_FILE 1
#define ERROR_NOT_ENOUGH_MEMORY 2
#define ERROR_INVALID_CMD_ARGUMENTS 3
#define ERROR_INVALID_CONFIG_FILE 4
#define MAX_RULES 255

typedef struct Config_rule {
	//specific for folder input rule
	char * input_folder;
	int min_src_samples; //min and max values are used to generate random number to cut up files in folder
	int max_src_samples;
	int min_src_offset;
	int max_src_offset;

	//specific for single file input rule
	char * input_file;
	int src_amount;
	int src_offset;

	//any rule can contain output path to write result of previous rules to disk
	char * output_file;

} Config_rule;

typedef struct Config {
	Config_rule rules[MAX_RULES];
	int count;
} Config;

int parse_config_file(Config * config, char * path)
{
	//Index of last rule stored. none are stored so init with -1
	config->count = -1;
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
			if (config->rules[config->count].input_folder) free(config->rules[config->count].input_folder);
			if (config->rules[config->count].output_file) free(config->rules[config->count].output_file);
			return ERROR_INVALID_CONFIG_FILE;
		}
		//Check what config rule we are dealing with
		else if (strcmp(key_buffer, "input_folder") == 0) {
			config->rules[config->count].input_folder = malloc(strlen(value_buffer) + 1);
			strcpy(config->rules[config->count].input_folder, value_buffer);
		} else if (strcmp(key_buffer, "input_file") == 0) {
			config->rules[config->count].input_file = malloc(strlen(value_buffer) + 1);
			strcpy(config->rules[config->count].input_file, value_buffer);
		}
		else if (strcmp(key_buffer, "output_file") == 0) {
			config->rules[config->count].output_file = malloc(strlen(value_buffer) + 1);
			strcpy(config->rules[config->count].output_file, value_buffer);
		}
		else if (strcmp(key_buffer, "min_src_samples") == 0) {
			config->rules[config->count].min_src_samples = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "max_src_samples") == 0) {

			config->rules[config->count].max_src_samples = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "min_src_offset") == 0) {
			offset = i;
			config->rules[config->count].min_src_offset = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "max_src_offset") == 0) {
			config->rules[config->count].max_src_offset = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "src_amount") == 0) {
			config->rules[config->count].src_amount = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "src_offset") == 0) {
			config->rules[config->count].src_offset = atoi(value_buffer);
		}
		else if (strcmp(key_buffer, "[") == 0) {
			//Start of new ruleset
			config->count++;
			config->rules[config->count].input_folder = NULL;
			config->rules[config->count].output_file = NULL;
			config->rules[config->count].input_file = NULL;
			config->rules[config->count].min_src_samples = 0;
			config->rules[config->count].max_src_samples = 0;
			config->rules[config->count].min_src_offset = 0;
			config->rules[config->count].max_src_offset = 0;
			config->rules[config->count].src_offset = 0;
			config->rules[config->count].src_amount = 0;
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
		 Config * config = malloc(sizeof(Config));
		 int result = parse_config_file(config, argv[1]);
		 if (result != 0) return result;
		 

		 Raw_wave * final_output = create_header();
		 for (i = 0; i < config->count+1; i++) {
			 Raw_wave * subassembly = NULL;
			 //If input file was given, take segment from that file according to other config rules
			 if (config->rules[i].input_file) {
				 Config_rule current_rule = config->rules[i];
				 //loading wave from disk will store entire file, so use tmp //TODO: params from load_wave to load partial file?
				 Raw_wave * tmp = NULL;
				 load_wave(&tmp, current_rule.input_file);
				 //if no configs are found, use default values
				 int src_amount = current_rule.src_amount > 0 ? current_rule.src_amount : num_samples(tmp);
				 int src_offset = current_rule.src_offset > 0 ? current_rule.src_offset : 0;
				 //get segment of loaded wave
				 subassembly = create_header();
				 insert_samples(subassembly, tmp, src_amount, current_rule.src_amount, 0, false);
			 }
			//If input folder was given take all files from that folder and merge all waves according to other config rules
			 else if (config->rules[i].input_folder) {
				 printf("Merging waves from input folder..\n");
				 Config_rule current_rule = config->rules[i];
				 subassembly = merge_waves_random(current_rule.input_folder, current_rule.min_src_samples, current_rule.max_src_samples, current_rule.min_src_offset, current_rule.max_src_offset);
			 }
			 //	glue subassembly to final_output
			 insert_samples(final_output, subassembly, num_samples(subassembly), 0, num_samples(final_output), false);
			 destroy_wave(&subassembly);

			 //if rule contains output, write final product there
			 if (config->rules[i].output_file) {
				 printf("Saving wave file to %s..\n", config->rules[i].output_file);
				 write_wave(final_output, config->rules[i].output_file);
			 }
			 //Config rule was parsed, clean it
			 if (config->rules[i].input_folder) free(config->rules[i].input_folder);
			 if (config->rules[i].output_file) free(config->rules[i].output_file);
			 if (config->rules[i].output_file) free(config->rules[i].input_file);
		 }
		 destroy_wave(&final_output);
		 free(config);
		 printf("Edn");
		 return EXIT_SUCCESS;
	 }
}

int interactive_input()
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
		char buffer_two[MAX_LEN_INPUT_STRING];

		Raw_wave * result = create_header();
		Raw_wave * wave_two = NULL;
		
		do {
			//If a new wave was already loaded in previous iteration of do while loop, don't ask for new one
			if (!wave_two){
				do {
					printf("Take samples from what file?\n");
					scanf("%s", buffer);
				} while (load_wave(&wave_two, buffer) < 0);
			}

			printf("Offset from source file (non-number for no offset)?\n");
			printf("and take how many samples (non-number for all)?\n");
			scanf("%s %s", buffer, buffer_two);
			int src_offset = atoi(buffer);

			int n_samples = 0;
			if (buffer_two[0] >= 'A' && buffer_two[0] <= 'z') {
				n_samples = num_samples(wave_two);
			} else {
				n_samples = atoi(buffer_two);
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

			insert_samples(result, wave_two_segment, num_samples(wave_two_segment), 0, insert_point, false);
			free(wave_two);
			wave_two = NULL;
			free(wave_two_segment);
			wave_two_segment = NULL;

			printf("Add another file?\n");
			scanf("%s", buffer);
		} while (load_wave(&wave_two, buffer) > 0 || buffer[0] != 'n');

		do {
			printf("Output file?\n");
			scanf("%s", buffer);
		} while (write_wave(result, buffer) < 0);

		free(result);
		result = NULL;
		return EXIT_SUCCESS;
	}
}