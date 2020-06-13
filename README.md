
# wav_hammer
Software for manipulating Waveform Audio File Format files.
## Usage
### Library example
```
Raw_wave* wav;
load_wave(&wav, input_path);
print_wave(wav);
Raw_wave* cpy =  create_header(samplerate(wav), bits_per_sample(wav), num_channels(wav));
insert_samples(
	.dst = cpy,
	.src = wav,
	.src_offset = num_samples(wav)  /  2,
	.src_amount = num_samples(wav)  /  2
);
write_wave(cpy, output_path);
destroy_wave(&cpy);

```
For more details see lib_example.c
### Binary
`wav_hammer steps.ini`
Takes a .ini file that defines actions being taken to edit .wav files. It uses following structure:
```
[=type of action
param_one=lotsofdingen
param_two=7234
```
Example:
```
[=single_file
input_file=../../audio/2.wav
src_amount=500000
src_offset=700000
effect=hamming_punch_distortion
[=output
num_channels=2
bits_per_sample=16
samplerate=441000
output_file=../../output/example.wav
```
for more details see `stuff/config info.txt` and `stuff/exampe_config.`
## Build on Linux
### Library
In src directory
```
make lib
sudo make install_lib
```
### Bin
In src directory
```
make bin
sudo make install_bin
```
## Build on Windows
Currently untested on windows,
You will atleast need a dirent.h`
For example:
https://web.archive.org/web/20170428133315/http://www.softagalleria.net/dirent.php

Place somewhere like
`C:\Program Files\Microsoft Visual Studio 9.0\VC\include`