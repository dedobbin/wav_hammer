#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <wav_hammer/wav_hammer.h>
#include <math.h>

int main(int argc, char* argv[])
{
	if (argc < 3){
		printf("Usage: lib_example input.wav output_folder\n");
		return 1;
	}

	char* input_path = argv[1];
	char* output_path = argv[2];

	srand(time(NULL));
	Raw_wave* wav;
	if (!load_wave(&wav, input_path)){
		printf("Could not load %s\n", input_path);
		return 1;
	}

	print_wave(wav, 1);

	// Copy half of samples
	Raw_wave* cpy = create_header(samplerate(wav), bits_per_sample(wav), num_channels(wav));
	insert_samples(  //Fancy named params
		.dst = cpy, 
		.src = wav,
		.src_offset = num_samples(wav) / 2,
		.src_amount = num_samples(wav) / 2
	);	

	char copy_path[100];
	strcpy(copy_path, output_path);
	strcpy(copy_path + strlen(output_path), "/lib_example_copy.wav");
	if (!write_wave(cpy, copy_path)){
		printf("Could write to %s\n", copy_path);
	}
	printf("Created %s\n", copy_path);

	//Change samplerate
	unsigned original_samplerate = samplerate(cpy);
	set_samplerate(cpy, original_samplerate * 2);
	
	char wrong_samplerate_path[100];
	strcpy(wrong_samplerate_path, output_path);
	strcpy(wrong_samplerate_path + strlen(output_path), "/lib_example_wrong_samplerate.wav");
	if (!write_wave(cpy, wrong_samplerate_path)){
		printf("Could write to %s\n", wrong_samplerate_path);
	}
	printf("Created %s\n", wrong_samplerate_path);

	set_samplerate(cpy, original_samplerate);

	// Insert random samples
	int max = pow(2, bits_per_sample(cpy));
	for (int i = 0; i < num_samples(cpy); i++){
		set_sample(cpy, i, rand() % max);	//TODO: dont hardcore for 16bits per sample
	}

	printf("-------%d\n", max);

	char white_noise_path[100];
	strcpy(white_noise_path, output_path);
	strcpy(white_noise_path + strlen(output_path), "/lib_example_white_noise.wav");
	if (!write_wave(cpy, white_noise_path)){
		printf("Could write to %s\n", white_noise_path);
	}
	printf("Created %s\n", white_noise_path);

	destroy_wave(&wav);
	destroy_wave(&cpy);
	return 0;
}