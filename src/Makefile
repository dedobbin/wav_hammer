all:		wavhammer clean

wavhammer:	linked_list.o wave_samples.o raw_wave.o hamming.o 
		gcc main.c linked_list.o wave_samples.o raw_wave.o hamming.o -o ../wavhammer.exe -lm

linked_list.o:
		gcc linked_list.c -c

wave_samples.o:
		gcc wave_samples.c -c

raw_wave.o:
		gcc raw_wave.c -c

hamming.o:	
		gcc hamming.c -c

clean:		
		rm *.o

stfu:		
		gcc *.c -o ../wavhammer.exe -lm -w -g
