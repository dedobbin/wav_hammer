#include "wave_samples.h"
#include "process_input.h"

int main(int argc, char* argv[])
{
	/**
	commandline arguments: 		src_offset	0	long

	1: input folder
	2: output file path
	3: minimal amount or random samples from input file
	4: maxium amount or random samples from input file
	5: minimal offset from input file
	6: maxium offset from input file

	OR 

	1: config file
	**/

	if (argc > 1) {
		process_commandline_arguments(argc, argv);
	} else {
		interactive_input();
	}
	getchar();
	return 0;
}

