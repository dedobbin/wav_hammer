all:		exe clean

exe:		wavhammer.a main.c
		gcc main.c wavhammer.a -o ../wavhammer.exe -lm -g

wavhammer.a:		linked_list.o wave_samples.o raw_wave.o distortion.o merge_waves.o utils.o process_input.o
		ar rcs wavhammer.a linked_list.o wave_samples.o raw_wave.o distortion.o merge_waves.o utils.o process_input.o

linked_list.o:
		gcc  linked_list.c -c -g

wave_samples.o:
		gcc wave_samples.c -c -g

raw_wave.o:
		gcc raw_wave.c -c -g

distortion.o:	
		gcc distortion.c -c -g

merge_waves.o:
		gcc merge_waves.c -c -g

utils.o:
		gcc utils.c -c -g

process_input.o:
		gcc process_input.c -c -g

clean:		
		rm *.o

stfu:		
		gcc *.c -o ../wavhammer.exe -lm -w -g
