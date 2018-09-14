#include <string.h>
#include <stdlib.h>
#include "process_input.h"
#include "wave_samples.h"
#include "raw_wave.h"
#include "distortion.h"
#include "datatypes.h"
#include "merge_waves.h"


#define EXIT_SUCCESS 0
#define ERROR_COULD_NOT_READ_FILE 1
#define ERROR_NOT_ENOUGH_MEMORY 2
#define ERROR_INVALID_CMD_ARGUMENTS 3
#define ERROR_INVALID_CONFIG_FILE 4
#define MAX_RULESETS 1000
#define MAX_STR_LEN 255

typedef struct Config_ruleset {
	
	char * mode;
	
	//rules for folder_random mode
	char * input_folder;
	long min_src_samples; //min and max values are used to generate random number to cut up files in folder
	long max_src_samples;
	long min_src_offset;
	long max_src_offset;
	int perc_skip;
	int times;

	//rules for single_file mode
	char * input_file;
	int src_amount;
	int src_offset;

	//rules for output mode
	char * output_file;
	int bits_per_sample;
	int samplerate;
	int num_channels;

	//multiple modes can contain
	char * effect;

} Config_ruleset;

typedef struct Config {
	Config_ruleset rulesets[MAX_RULESETS];
	int count;
} Config;

void process_effect_rule(Raw_wave * target, char * rule)
{
	char effect_type[MAX_STR_LEN];
	int effect_param = 0;
	//check if contains param
	char* pos1 = strchr(rule, '(');
	char* pos2 = strchr(rule, ')');
	if (pos1 && pos2) {
		//effects that need param
		if (rule && sscanf(rule, "%s (%d)", effect_type, &effect_param) > 0) {
			if (strcmp(effect_type, "distortion") == 0) {
				distortion_multiply(target, effect_param);
			}
			else if (strcmp(effect_type, "hamming_distortion") == 0) {
				hamming_distortion(target, effect_param);
			}
			else {
				printf("process_effect_rule: invalid effect rule: %s\n", rule);
			}
		}
	}
	else {
		//effect that don't use param
		if (strcmp(rule, "hamming_kapot") == 0) {
			hamming_kapot(target);
		}
		else if (strcmp(rule, "hamming_punch_distortion") == 0) {
			hamming_punch_distortion(target);
		}
		else if (strcmp(rule, "hamming_pointless_distortion") == 0) {
			hamming_pointless_distortion(target);
		}
		else if (strcmp(rule, "hamming_pointless_distortion2") == 0) {
			hamming_pointless_distortion2(target);
		}
		else if (strcmp(rule, "hamming_pointless_distortion3") == 0) {
			hamming_pointless_distortion3(target);
		}
		else {
			printf("process_effect_rule: invalid effect rule: %s\n", rule);
		}
	}
}

int parse_config_file(Config * config, char * path)
{
	//Index of last ruleset stored. none are stored so init with -1
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
	char key_buffer[MAX_STR_LEN];
	char value_buffer[MAX_STR_LEN];
	int i = 0;
	char c;
	do {
		memset(key_buffer, 0, MAX_STR_LEN);
		memset(value_buffer, 0, MAX_STR_LEN);

		//Read until a complete config 'key' is in buffer
		int offset = i;
		do {
			c = file_buffer[i];
			key_buffer[i - offset] = c;
			i++;
		} while (c != 61 && c != '\0'); //61 is ascii code for '='
		key_buffer[i - offset - 1] = '\0'; //= is also memcpy'd, just overwrite it

										   //If there was a space before key, strip it out TODO: clean up this code sometime 
#ifdef _WIN32
		if (key_buffer[0] == '\r' && key_buffer[1] == '\n') {
			strcpy(key_buffer, key_buffer + 2);
		}
#else
		if (key_buffer[0] == '\n') {
			strcpy(key_buffer, key_buffer + 1);
		}
#endif

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
		if (strcmp(key_buffer, "[") == 0) {
			//Start of new ruleset
			config->count++;

			config->rulesets[config->count].mode = malloc(strlen(value_buffer) + 1);
			strcpy(config->rulesets[config->count].mode, value_buffer);

			config->rulesets[config->count].input_folder = NULL;
			config->rulesets[config->count].output_file = NULL;
			config->rulesets[config->count].input_file = NULL;
			config->rulesets[config->count].effect = NULL;
			config->rulesets[config->count].min_src_samples = 0;
			config->rulesets[config->count].max_src_samples = 0;
			config->rulesets[config->count].min_src_offset = 0;
			config->rulesets[config->count].max_src_offset = 0;
			config->rulesets[config->count].src_offset = 0;
			config->rulesets[config->count].src_amount = 0;
			config->rulesets[config->count].perc_skip = 0;
			config->rulesets[config->count].times = 0;
			config->rulesets[config->count].samplerate = 0;
			config->rulesets[config->count].bits_per_sample = 0;
			config->rulesets[config->count].num_channels = 0;
		}
		else if (strcmp(key_buffer, "") == 0 || strcmp(value_buffer, "") == 0) {
			if (config->rulesets[config->count].input_folder) free(config->rulesets[config->count].input_folder);
			if (config->rulesets[config->count].output_file) free(config->rulesets[config->count].output_file);
			return ERROR_INVALID_CONFIG_FILE;
		}
		//single file mode
		else if (strcmp(config->rulesets[config->count].mode, "single_file") == 0) {
			if (strcmp(key_buffer, "effect") == 0) {
				config->rulesets[config->count].effect = malloc(strlen(value_buffer) + 1);
				strcpy(config->rulesets[config->count].effect, value_buffer);
			}
			else if (strcmp(key_buffer, "input_file") == 0) {
				config->rulesets[config->count].input_file = malloc(strlen(value_buffer) + 1);
				strcpy(config->rulesets[config->count].input_file, value_buffer);
			}
			else if (strcmp(key_buffer, "src_amount") == 0) {
				config->rulesets[config->count].src_amount = atoi(value_buffer);
			}
			else if (strcmp(key_buffer, "src_offset") == 0) {
				config->rulesets[config->count].src_offset = atoi(value_buffer);
			} else {
				printf("Invalid rule: %s\n", key_buffer);
			}
		}
		//folder random mode
		else if (strcmp(config->rulesets[config->count].mode, "folder_random") == 0) {
			if (strcmp(key_buffer, "input_folder") == 0) {
				config->rulesets[config->count].input_folder = malloc(strlen(value_buffer) + 1);
				strcpy(config->rulesets[config->count].input_folder, value_buffer);
			}
			else if (strcmp(key_buffer, "min_src_samples") == 0) {
				config->rulesets[config->count].min_src_samples = atol(value_buffer);
			}
			else if (strcmp(key_buffer, "max_src_samples") == 0) {
				config->rulesets[config->count].max_src_samples = atol(value_buffer);
			}
			else if (strcmp(key_buffer, "min_src_offset") == 0) {
				config->rulesets[config->count].min_src_offset = atol(value_buffer);
			}
			else if (strcmp(key_buffer, "max_src_offset") == 0) {
				config->rulesets[config->count].max_src_offset = atol(value_buffer);
			}
			else if (strcmp(key_buffer, "perc_skip") == 0) {
				config->rulesets[config->count].perc_skip = atoi(value_buffer);
			}
			else if (strcmp(key_buffer, "times") == 0) {
				config->rulesets[config->count].times = atoi(value_buffer);
			}
			else if (strcmp(key_buffer, "effect") == 0) {
				config->rulesets[config->count].effect = malloc(strlen(value_buffer) + 1);
				strcpy(config->rulesets[config->count].effect, value_buffer);
			} else {
				printf("Invalid rule: %s\n", key_buffer);
			}
		}
		//output mode
		else if (strcmp(config->rulesets[config->count].mode, "output") == 0) {
			if (strcmp(key_buffer, "output_file") == 0) {
				config->rulesets[config->count].output_file = malloc(strlen(value_buffer) + 1);
				strcpy(config->rulesets[config->count].output_file, value_buffer);
			}
			else if (strcmp(key_buffer, "bits_per_sample") == 0) {
				config->rulesets[config->count].bits_per_sample = atoi(value_buffer);
			}
			else if (strcmp(key_buffer, "samplerate") == 0) {
				config->rulesets[config->count].samplerate = atoi(value_buffer);
			} 
			else if (strcmp(key_buffer, "num_channels") == 0) {
				config->rulesets[config->count].num_channels = atoi(value_buffer);
			} else {
				printf("Invalid rule: %s\n", key_buffer);
			}
		} 
		else {
			printf("Invalid mode: %s\n", key_buffer);
		}
	} while (c != '\0');

	printf("parse_config_file: Found %d rulesets\n", config->count + 1);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc == 2) {
		int i = 0;
		Config * config = malloc(sizeof(Config));
		int result = parse_config_file(config, argv[1]);
		if (result != 0) {
			if (result == ERROR_INVALID_CONFIG_FILE) {
				printf("Invalid config file, cannot parse\n");
			}
			return result;
		};

		Raw_wave * final_output = create_header(44800, 16);
		for (i = 0; i < config->count + 1; i++) {
			Config_ruleset current_ruleset = config->rulesets[i];
			printf("Processing ruleset %d: %s mode..\n", i + 1, current_ruleset.mode);
			Raw_wave * subassembly = NULL;

			if (strcmp(current_ruleset.mode, "single_file") == 0) {
				//process singe_file ruleset
				if (current_ruleset.src_amount == 0) {
					printf("single_file ruleset using 0 src samples, skipping..\n");
					continue;
				}

				//loading wave from disk will store entire file, so use tmp //TODO: params from load_wave to load partial file?
				Raw_wave * tmp = NULL;
				load_wave(&tmp, current_ruleset.input_file);
				//if no configs are found, use default values
				int src_amount = current_ruleset.src_amount > 0 ? current_ruleset.src_amount : num_samples(tmp);
				int src_offset = current_ruleset.src_offset > 0 ? current_ruleset.src_offset : 0;

				//get segment of loaded wave
				subassembly = create_header(44800, 16);
				insert_samples(.dst = subassembly, .src = tmp, .src_amount = current_ruleset.src_amount, .src_offset = current_ruleset.src_offset);

				//Check if need to apply effect
				if (current_ruleset.effect) {
					process_effect_rule(subassembly, current_ruleset.effect);
				}
				//glue subassembly to final_output, unless it's output rule, there is no new data then
				insert_samples(.dst = final_output, .src = subassembly, .src_amount = num_samples(subassembly));
				destroy_wave(&subassembly);

			}

			else if (strcmp(current_ruleset.mode, "folder_random") == 0) {
				//process folder_random mode ruleset
				printf("Merging waves from input folder..\n");
				subassembly = merge_waves_random(
					current_ruleset.input_folder, current_ruleset.min_src_samples,
					current_ruleset.max_src_samples, current_ruleset.min_src_offset,
					current_ruleset.max_src_offset, current_ruleset.perc_skip, current_ruleset.times
				);
				if (current_ruleset.effect) {
					process_effect_rule(subassembly, current_ruleset.effect);
				}
				//glue subassembly to final_output, unless it's output rule, there is no new data then
				insert_samples(.dst = final_output, .src = subassembly, .src_amount = num_samples(subassembly));
				destroy_wave(&subassembly);
			}

			else if (strcmp(current_ruleset.mode, "output") == 0) {
				//process output mode ruleset
				if (current_ruleset.bits_per_sample)
					set_bits_per_sample(final_output, current_ruleset.bits_per_sample);
				if (current_ruleset.samplerate)
					set_samplerate(final_output, current_ruleset.samplerate);
				if (current_ruleset.num_channels)
					set_num_channels(final_output, current_ruleset.num_channels);
				printf("Saving wave file to %s (Samplerate: %d, %d bits per sample, %d channel(s))..\n", current_ruleset.output_file, samplerate(final_output), bits_per_sample(final_output), num_channels(final_output));
				write_wave(final_output, current_ruleset.output_file);
			}

			//Config rule was parsed, clean it
			if (config->rulesets[i].input_folder) free(config->rulesets[i].input_folder);
			if (config->rulesets[i].output_file) free(config->rulesets[i].output_file);
			if (config->rulesets[i].output_file) free(config->rulesets[i].input_file);
			if (config->rulesets[i].effect) free(config->rulesets[i].effect);
			if (config->rulesets[i].mode) free(config->rulesets[i].mode);
		}
		destroy_wave(&final_output);
		free(config);
		printf("Edn\n");
	} else {
		printf("Give path to config file as commandline argument\n");
	}
	getchar();
	return EXIT_SUCCESS;
}

