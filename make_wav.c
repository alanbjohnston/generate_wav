/* make_wav.c
 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 * 
 * Edited by Dolin Sergey. dlinyj@gmail.com
 * April 11 12:58 2014
 */
 
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "make_wav.h"
 
void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file)
{
	unsigned buf;
	while(num_bytes>0)
	{   buf = word & 0xff;
		fwrite(&buf, 1,1, wav_file);
		num_bytes--;
	word >>= 8;
	}
}
 
/* information about the WAV file format from
 
http://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 
 */
 
void write_wav(char * filename, unsigned long num_samples, short int * data, int s_rate)
{
	FILE* wav_file;
	unsigned int sample_rate;
	unsigned int num_channels;
	unsigned int bytes_per_sample;
	unsigned int byte_rate;
	unsigned long i;    /* counter for samples */
 
	num_channels = 1;   /* monoaural */
	bytes_per_sample = 2;
 
	if (s_rate<=0) sample_rate = 44100;
	else sample_rate = (unsigned int) s_rate;
 
	byte_rate = sample_rate*num_channels*bytes_per_sample;
 
	wav_file = fopen(filename, "w");
	assert(wav_file);   /* make sure it opened */
 
	/* write RIFF header */
	fwrite("RIFF", 1, 4, wav_file);
	write_little_endian(36 + bytes_per_sample* num_samples*num_channels, 4, wav_file);
	fwrite("WAVE", 1, 4, wav_file);
 
	/* write fmt  subchunk */
	fwrite("fmt ", 1, 4, wav_file);
	write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
	write_little_endian(1, 2, wav_file);    /* PCM is format 1 */
	write_little_endian(num_channels, 2, wav_file);
	write_little_endian(sample_rate, 4, wav_file);
	write_little_endian(byte_rate, 4, wav_file);
	write_little_endian(num_channels*bytes_per_sample, 2, wav_file);  /* block align */
	write_little_endian(8*bytes_per_sample, 2, wav_file);  /* bits/sample */
 
	/* write data subchunk */
	fwrite("data", 1, 4, wav_file);
	write_little_endian(bytes_per_sample* num_samples*num_channels, 4, wav_file);
	
	for (i=0; i< num_samples; i++)
	{   write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
	}
 
	fclose(wav_file);
}


#define S_RATE  (48000)     // (44100)
#define BUF_SIZE (S_RATE*10) /* 2 second buffer */
 
//int buffer[BUF_SIZE];
short int buffer[BUF_SIZE];

double d_random(double min, double max)
{
    return min + (max - min) / RAND_MAX * rand();
}
 
int main(int argc, char * argv[])
{
	int i;
	//float amplitude = 32000;
	float amplitude = 20000; // 32767/(10%amp+5%amp+100%amp)
//	float freq_Hz = 100;
	float freq_Hz = 3000;
	
	char b[128] = {0x07,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x7E,0x03,0x00,0x00,0x00,0x00,0xE6,0x01,0x00,0x27,0xD1,0x02,
		        0xE5,0x40,0x04,0x18,0xE1,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		        0x00,0x00,0x00,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		        0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		        0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		        0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		        0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A		     };
	
	srand((unsigned int)time(0));
	
	int buf_len = 128*8*16;
	
	//short int meandr_value=32767;
        int phase = 1;
	/* fill buffer with a sine wave */
//	for (i=0; i<BUF_SIZE; i++)
	for (i=0; i< buf_len; i++)
	{
		if ( (i % 16) == 0) {
//			if (rand() > RAND_MAX/2) {  // binary 1
//			if (b[i % (16 * 8)] & 1 << ((i % 16)-1)) {  // check byte x bit y value
//			if (((int)(i / (16 * 8))) & 1 << ((i - (int)(i / (16 * 8))) - 1))  { // check byte x bit y value
			int byte = ((int)(i / (16 * 8)));
			int bit = i - byte * 16 * 8;
			printf ("b[%d] bit %d = %d \n", byte, bit, (byte & 1<<(bit-1)));
			{
			
				phase *= -1;
			}
		}
		buffer[i] = (int)(amplitude/2 * phase * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); //10%amp
	//	buffer[i] +=(int)(amplitude/20 * sin((float)(2*M_PI*i*10*freq_Hz/S_RATE))); //5% amp
	//	buffer[i] +=(int)(amplitude * sin((float)(2*M_PI*i*100*freq_Hz/S_RATE))); //100% amp
		
		//buffer[i] +=(int)(amplitude * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); //100% amp
		
		//buffer[i] +=(int)amplitude/10*d_random(-1.0, 1.0); //nois
		
		/*
		//meandr
		if (!(i%(S_RATE/((int)freq_Hz/2)))) {
			if (meandr_value==32767) {
				meandr_value=-32767;
			} else { 
				meandr_value=32767;
			}
		}
		buffer[i]=meandr_value;
		*/
	}
	write_wav("test.wav", buf_len, buffer, S_RATE);
 
	return 0;
}
