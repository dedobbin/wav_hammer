all:		wavhammer clean

wavhammer:	linked_list.o wave_samples.o raw_wave.o distortion.o merge_waves.o utils.o process_input.o 
		gcc main.c linked_list.o wave_samples.o raw_wave.o distortion.o merge_waves.o utils.o process_input.o -o ../wavhammer.exe -lm

linked_list.o:
		gcc linked_list.c -c

wave_samples.o:
		gcc wave_samples.c -c

raw_wave.o:
		gcc raw_wave.c -c

distortion.o:	
		gcc distortion.c -c

merge_waves.o:
		gcc merge_waves.c -c

utils.o:
		gcc utils.c -c

process_input.o:
		gcc process_input.c -c

clean:		
		rm *.o

stfu:		
		gcc *.c -o ../wavhammer.exe -lm -w -g
