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
	Raw_wave * wave = merge_waves();	
	write_wave(wave, "../../audio/output.wav");
	**/

	Raw_wave * container = create_header();
	Raw_wave * wave;
	load_wave(&wave, "../../audio/1.wav");
	int srcAmount = 60000;
	//int srcOffset = random(10000, 60000);
	insert_samples(container, wave, srcAmount, 0, 0, false);
	print_wave(container);

	destroy_wave(&wave);
	destroy_wave(&container);
	printf("------------------------------------------------------------------------\n");
	Raw_wave * containerTwo = create_header();
	load_wave(&wave, "../../audio/1.wav");
	srcAmount = 240000;
	//int srcOffset = random(10000, 60000);
	int srcOffset = num_samples(wave) /2;
	insert_samples(containerTwo, wave, srcAmount, srcOffset, 0, false);

	write_wave(containerTwo, "../../audio/output.wav");


	print_wave(containerTwo);


	/**
	Linked_list * list = llist_create();
	llist_push(&list, 0);
	llist_push(&list, 1);
	llist_push(&list, 2);
	llist_print(&list);

	printf("loading .wavs..\n");
	Raw_wave * waveOne;
	if (!load_wave(&waveOne, "../../audio/4.wav")) {
		printf("Could not init wave one");
	}

	Raw_wave * waveTwo;
	if (!load_wave(&waveTwo, "../../audio/5.wav")) {
		printf("Could not init wave two");
	}

	printf("===================\n");
	print_wave(waveTwo);
	printf("===================\n");

	printf("merging .wavs..\n");
	//insert_samples(waveOne, waveTwo, num_samples(waveTwo), num_samples(waveTwo)/2, num_samples(waveOne) /2, false);
	int srcAmount = 60000;
	//int srcOffset = random(10000, 60000);
	int srcOffset = num_samples(waveOne) / 2;
	insert_samples(waveOne, waveTwo, srcAmount, srcOffset, num_samples(waveOne), false);

	printf("===================\n");
	print_wave(waveOne);
	printf("===================\n");

	printf("Writing .wav to disk..\n");
	write_wave(waveOne, "../../audio/output.wav");

	printf("Done\n");
	**/
	getchar();	

	return 0;

}

