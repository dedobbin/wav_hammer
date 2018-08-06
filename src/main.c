#include <stdio.h>
#ifdef __linux__ 
#include <unistd.h>
#elif _WIN32
#endif
#include <string.h>
#include "raw_wave.h"
#include "hamming.h"
#include "datatypes.h"
#include "merge_waves.h"

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
	**/

	int result = 0;
	if (argc != 7) {
		printf("No arguments given..\n");
		printf("Commandline arguments:\n");
		printf("1:\tinput folder\n2: \toutput file path\n3: \tminimal amount or random samples from input file\n4: \tmaxium amount or random samples from input file\n5: \tminimal offset from input file\n6: \tmaxium offset from input file");
		result = 1;
	} else {
		printf("Merging waves..\n");
		Raw_wave * wave = merge_waves(argv[1], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
		printf("Saving wave file to %s..\n", argv[2]);
		write_wave(wave, argv[2]);
		destroy_wave(&wave);
		printf("Edn\n");
		result = 0;
	}
	return result;

}

