install: install_lib install_bin clean

install_lib: lib
	cp libwav_hammer.so /usr/local/lib/; 	\
	cp raw_wave.h wave_samples.h wh_distortion.h wh_datatypes.h merge_waves.h /usr/local/include/;	\
	ldconfig

install_bin: bin
	cp wav_hammer /usr/local/bin/; 	\

bin:	main.o
	gcc main.o -o wav_hammer -lwav_hammer

main.o: main.c
	gcc  main.c -c -g

lib:	wh_linked_list.o wave_samples.o raw_wave.o wh_distortion.o merge_waves.o wh_linked_list.o wh_utils.o
	gcc -shared wh_linked_list.o wave_samples.o raw_wave.o wh_distortion.o merge_waves.o wh_utils.o -o libwav_hammer.so -lm

wh_linked_list.o: wh_linked_list.c
	gcc  wh_linked_list.c -c -g -fPIC

wave_samples.o: wave_samples.c raw_wave.c
		gcc wave_samples.c -c -g -fPIC

raw_wave.o:	raw_wave.c wave_samples.c
	gcc raw_wave.c -c -g -fPIC

wh_distortion.o: wh_distortion.c wave_samples.c wh_utils.c raw_wave.c
	gcc wh_distortion.c -c -g -fPIC

merge_waves.o: merge_waves.c wave_samples.c wh_utils.c
	gcc merge_waves.c -c -g -fPIC

wh_utils.o: wh_utils.c
	gcc wh_utils.c -c -g -fPIC

clean:
	rm *.o *.so wav_hammer

uninstall:
	rm /usr/local/bin/wavhammer
	rm /usr/local/lib/libwav_hammer.so; \
	rm  /usr/local/include/raw_wave.h; \
	rm  /usr/local/include/wave_samples.h; \
	rm  /usr/local/include/merge_waves.h; \
	rm  /usr/local/include/wh_distortion.h; \
	rm  /usr/local/include/wh_datatypes.h; \

