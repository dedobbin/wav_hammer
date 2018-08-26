#include "process_input.h"
#include "raw_wave.h"
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

	//void insert_samples(Raw_wave * dst, Raw_wave * src, long src_amount, long src_offset, long dst_offset, bool overwrite);
	Raw_wave * total = NULL;
	load_wave(&total, "../../audio/1.wav");
	
	Raw_wave * segment = create_header();
	insert_samples(segment, total, num_samples(total), 0, 0, false);
	write_wave(segment, "../../output/testing.wav");
	printf("num samples input : %d\n", num_samples(total));
	printf("num samples output : %d\n", num_samples(segment));
	getchar();
	return 0;

	if (argc > 1) {
		process_commandline_arguments(argc, argv);
	} else {
		interactive_input();
	}
	getchar();
	return 0;
}

