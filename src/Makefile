all: bin lib

install: install_lib install_bin clean

install_lib: lib
	mkdir -p /usr/local/lib/wav_hammer;	\
	mkdir -p /usr/local/include/wav_hammer;	\
	cp libwav_hammer.so /usr/local/lib/wav_hammer; 	\
	cp wav_hammer.h raw_wave.h wave_samples.h distortion.h datatypes.h merge_waves.h /usr/local/include/wav_hammer/;

install_bin: bin
	cp wav_hammer /usr/local/bin/; 	\

bin:	main.o
	gcc main.o -o wav_hammer -lwav_hammer

lib_example: lib_example.c
	gcc lib_example.c -o lib_example -lwav_hammer -lm

main.o: main.c
	gcc  main.c -c

lib:	linked_list.o wave_samples.o raw_wave.o distortion.o merge_waves.o linked_list.o utils.o
	gcc -shared linked_list.o wave_samples.o raw_wave.o distortion.o merge_waves.o utils.o -o libwav_hammer.so -lm

linked_list.o: linked_list.c
	gcc  linked_list.c -c -fPIC

wave_samples.o: wave_samples.c raw_wave.c
		gcc wave_samples.c -c -fPIC

raw_wave.o:	raw_wave.c wave_samples.c
	gcc raw_wave.c -c -fPIC
distortion.o: distortion.c wave_samples.c utils.c raw_wave.c
	gcc distortion.c -c -fPIC

merge_waves.o: merge_waves.c wave_samples.c utils.c
	gcc merge_waves.c -c -fPIC

utils.o: utils.c
	gcc utils.c -c -fPIC

clean:
	rm *.o *.so wav_hammer

uninstall_bin:
	rm /usr/local/bin/wav_hammer

uninstall_lib:
	rm -fr /usr/local/lib/wav_hammer; \
	rm  -fr /usr/local/include/wav_hammer; \

