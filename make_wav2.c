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
 
 // gcc -o make_enc_wav make_enc_wav.c -lm
 // ./make_enc_wav
 
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "make_wav.h"

#define SYNC_WORD 0b1000111110011010010000101011101 
#define false 0
#define true 1

/**
 * 
 * 8b10b code is from FoxTelem by Burns Fisher WB1FJ
 *
 * https://github.com/ac2cz/FoxTelem/blob/master/src/decoder/Code8b10b.java  
 *
 * FOX 1 Telemetry Decoder
 * @author chris.e.thompson g0kla/ac2cz
 *
 * Copyright (C) 2015 amsat.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General  License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General  License for more details.
 *
 * You should have received a copy of the GNU General  License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 * Static variables and methods to encode and decode 8b10b
 * 
 *
 */

static int getNextRd(int word, int flip);
static int getRdSense10b(int word, int flip);
static char decode(int word, int flip);
static void printAll10bwords();
static int encode(int word);

	 static  int NOT_FRAME = /* 0fa */ 0xfa & 0x3ff;
	 static  int FRAME = /* 0fa */ ~0xfa & 0x3ff;
	
	 static  int Encode_8b10b[][256] = {
		   // RD = -1 cases
		{
		   /* 00 */ 0x274,
		   /* 01 */ 0x1d4,
		   /* 02 */ 0x2d4,
		   /* 03 */ 0x71b,
		   /* 04 */ 0x354,
		   /* 05 */ 0x69b,
		   /* 06 */ 0x59b,
		   /* 07 */ 0x78b,
		   /* 08 */ 0x394,
		   /* 09 */ 0x65b,
		   /* 0a */ 0x55b,
		   /* 0b */ 0x74b,
		   /* 0c */ 0x4db,
		   /* 0d */ 0x6cb,
		   /* 0e */ 0x5cb,
		   /* 0f */ 0x174,
		   /* 10 */ 0x1b4,
		   /* 11 */ 0x63b,
		   /* 12 */ 0x53b,
		   /* 13 */ 0x72b,
		   /* 14 */ 0x4bb,
		   /* 15 */ 0x6ab,
		   /* 16 */ 0x5ab,
		   /* 17 */ 0x3a4,
		   /* 18 */ 0x334,
		   /* 19 */ 0x66b,
		   /* 1a */ 0x56b,
		   /* 1b */ 0x364,
		   /* 1c */ 0x4eb,
		   /* 1d */ 0x2e4,
		   /* 1e */ 0x1e4,
		   /* 1f */ 0x2b4,
		   /* 20 */ 0x679,
		   /* 21 */ 0x5d9,
		   /* 22 */ 0x6d9,
		   /* 23 */ 0x319,
		   /* 24 */ 0x759,
		   /* 25 */ 0x299,
		   /* 26 */ 0x199,
		   /* 27 */ 0x389,
		   /* 28 */ 0x799,
		   /* 29 */ 0x259,
		   /* 2a */ 0x159,
		   /* 2b */ 0x349,
		   /* 2c */ 0x0d9,
		   /* 2d */ 0x2c9,
		   /* 2e */ 0x1c9,
		   /* 2f */ 0x579,
		   /* 30 */ 0x5b9,
		   /* 31 */ 0x239,
		   /* 32 */ 0x139,
		   /* 33 */ 0x329,
		   /* 34 */ 0x0b9,
		   /* 35 */ 0x2a9,
		   /* 36 */ 0x1a9,
		   /* 37 */ 0x7a9,
		   /* 38 */ 0x739,
		   /* 39 */ 0x269,
		   /* 3a */ 0x169,
		   /* 3b */ 0x769,
		   /* 3c */ 0x0e9,
		   /* 3d */ 0x6e9,
		   /* 3e */ 0x5e9,
		   /* 3f */ 0x6b9,
		   /* 40 */ 0x675,
		   /* 41 */ 0x5d5,
		   /* 42 */ 0x6d5,
		   /* 43 */ 0x315,
		   /* 44 */ 0x755,
		   /* 45 */ 0x295,
		   /* 46 */ 0x195,
		   /* 47 */ 0x385,
		   /* 48 */ 0x795,
		   /* 49 */ 0x255,
		   /* 4a */ 0x155,
		   /* 4b */ 0x345,
		   /* 4c */ 0x0d5,
		   /* 4d */ 0x2c5,
		   /* 4e */ 0x1c5,
		   /* 4f */ 0x575,
		   /* 50 */ 0x5b5,
		   /* 51 */ 0x235,
		   /* 52 */ 0x135,
		   /* 53 */ 0x325,
		   /* 54 */ 0x0b5,
		   /* 55 */ 0x2a5,
		   /* 56 */ 0x1a5,
		   /* 57 */ 0x7a5,
		   /* 58 */ 0x735,
		   /* 59 */ 0x265,
		   /* 5a */ 0x165,
		   /* 5b */ 0x765,
		   /* 5c */ 0x0e5,
		   /* 5d */ 0x6e5,
		   /* 5e */ 0x5e5,
		   /* 5f */ 0x6b5,
		   /* 60 */ 0x673,
		   /* 61 */ 0x5d3,
		   /* 62 */ 0x6d3,
		   /* 63 */ 0x31c,
		   /* 64 */ 0x753,
		   /* 65 */ 0x29c,
		   /* 66 */ 0x19c,
		   /* 67 */ 0x38c,
		   /* 68 */ 0x793,
		   /* 69 */ 0x25c,
		   /* 6a */ 0x15c,
		   /* 6b */ 0x34c,
		   /* 6c */ 0x0dc,
		   /* 6d */ 0x2cc,
		   /* 6e */ 0x1cc,
		   /* 6f */ 0x573,
		   /* 70 */ 0x5b3,
		   /* 71 */ 0x23c,
		   /* 72 */ 0x13c,
		   /* 73 */ 0x32c,
		   /* 74 */ 0x0bc,
		   /* 75 */ 0x2ac,
		   /* 76 */ 0x1ac,
		   /* 77 */ 0x7a3,
		   /* 78 */ 0x733,
		   /* 79 */ 0x26c,
		   /* 7a */ 0x16c,
		   /* 7b */ 0x763,
		   /* 7c */ 0x0ec,
		   /* 7d */ 0x6e3,
		   /* 7e */ 0x5e3,
		   /* 7f */ 0x6b3,
		   /* 80 */ 0x272,
		   /* 81 */ 0x1d2,
		   /* 82 */ 0x2d2,
		   /* 83 */ 0x71d,
		   /* 84 */ 0x352,
		   /* 85 */ 0x69d,
		   /* 86 */ 0x59d,
		   /* 87 */ 0x78d,
		   /* 88 */ 0x392,
		   /* 89 */ 0x65d,
		   /* 8a */ 0x55d,
		   /* 8b */ 0x74d,
		   /* 8c */ 0x4dd,
		   /* 8d */ 0x6cd,
		   /* 8e */ 0x5cd,
		   /* 8f */ 0x172,
		   /* 90 */ 0x1b2,
		   /* 91 */ 0x63d,
		   /* 92 */ 0x53d,
		   /* 93 */ 0x72d,
		   /* 94 */ 0x4bd,
		   /* 95 */ 0x6ad,
		   /* 96 */ 0x5ad,
		   /* 97 */ 0x3a2,
		   /* 98 */ 0x332,
		   /* 99 */ 0x66d,
		   /* 9a */ 0x56d,
		   /* 9b */ 0x362,
		   /* 9c */ 0x4ed,
		   /* 9d */ 0x2e2,
		   /* 9e */ 0x1e2,
		   /* 9f */ 0x2b2,
		   /* a0 */ 0x67a,
		   /* a1 */ 0x5da,
		   /* a2 */ 0x6da,
		   /* a3 */ 0x31a,
		   /* a4 */ 0x75a,
		   /* a5 */ 0x29a,
		   /* a6 */ 0x19a,
		   /* a7 */ 0x38a,
		   /* a8 */ 0x79a,
		   /* a9 */ 0x25a,
		   /* aa */ 0x15a,
		   /* ab */ 0x34a,
		   /* ac */ 0x0da,
		   /* ad */ 0x2ca,
		   /* ae */ 0x1ca,
		   /* af */ 0x57a,
		   /* b0 */ 0x5ba,
		   /* b1 */ 0x23a,
		   /* b2 */ 0x13a,
		   /* b3 */ 0x32a,
		   /* b4 */ 0x0ba,
		   /* b5 */ 0x2aa,
		   /* b6 */ 0x1aa,
		   /* b7 */ 0x7aa,
		   /* b8 */ 0x73a,
		   /* b9 */ 0x26a,
		   /* ba */ 0x16a,
		   /* bb */ 0x76a,
		   /* bc */ 0x0ea,
		   /* bd */ 0x6ea,
		   /* be */ 0x5ea,
		   /* bf */ 0x6ba,
		   /* c0 */ 0x676,
		   /* c1 */ 0x5d6,
		   /* c2 */ 0x6d6,
		   /* c3 */ 0x316,
		   /* c4 */ 0x756,
		   /* c5 */ 0x296,
		   /* c6 */ 0x196,
		   /* c7 */ 0x386,
		   /* c8 */ 0x796,
		   /* c9 */ 0x256,
		   /* ca */ 0x156,
		   /* cb */ 0x346,
		   /* cc */ 0x0d6,
		   /* cd */ 0x2c6,
		   /* ce */ 0x1c6,
		   /* cf */ 0x576,
		   /* d0 */ 0x5b6,
		   /* d1 */ 0x236,
		   /* d2 */ 0x136,
		   /* d3 */ 0x326,
		   /* d4 */ 0x0b6,
		   /* d5 */ 0x2a6,
		   /* d6 */ 0x1a6,
		   /* d7 */ 0x7a6,
		   /* d8 */ 0x736,
		   /* d9 */ 0x266,
		   /* da */ 0x166,
		   /* db */ 0x766,
		   /* dc */ 0x0e6,
		   /* dd */ 0x6e6,
		   /* de */ 0x5e6,
		   /* df */ 0x6b6,
		   /* e0 */ 0x271,
		   /* e1 */ 0x1d1,
		   /* e2 */ 0x2d1,
		   /* e3 */ 0x71e,
		   /* e4 */ 0x351,
		   /* e5 */ 0x69e,
		   /* e6 */ 0x59e,
		   /* e7 */ 0x78e,
		   /* e8 */ 0x391,
		   /* e9 */ 0x65e,
		   /* ea */ 0x55e,
		   /* eb */ 0x74e,
		   /* ec */ 0x4de,
		   /* ed */ 0x6ce,
		   /* ee */ 0x5ce,
		   /* ef */ 0x171,
		   /* f0 */ 0x1b1,
		   /* f1 */ 0x637,
		   /* f2 */ 0x537,
		   /* f3 */ 0x72e,
		   /* f4 */ 0x4b7,
		   /* f5 */ 0x6ae,
		   /* f6 */ 0x5ae,
		   /* f7 */ 0x3a1,
		   /* f8 */ 0x331,
		   /* f9 */ 0x66e,
		   /* fa */ 0x56e,
		   /* fb */ 0x361,
		   /* fc */ 0x4ee,
		   /* fd */ 0x2e1,
		   /* fe */ 0x1e1,
		   /* ff */ 0x2b1,
		},   // RD = +1 cases 
		{
		   /* 00 */ 0x58b,
		   /* 01 */ 0x62b,
		   /* 02 */ 0x52b,
		   /* 03 */ 0x314,
		   /* 04 */ 0x4ab,
		   /* 05 */ 0x294,
		   /* 06 */ 0x194,
		   /* 07 */ 0x074,
		   /* 08 */ 0x46b,
		   /* 09 */ 0x254,
		   /* 0a */ 0x154,
		   /* 0b */ 0x344,
		   /* 0c */ 0x0d4,
		   /* 0d */ 0x2c4,
		   /* 0e */ 0x1c4,
		   /* 0f */ 0x68b,
		   /* 10 */ 0x64b,
		   /* 11 */ 0x234,
		   /* 12 */ 0x134,
		   /* 13 */ 0x324,
		   /* 14 */ 0x0b4,
		   /* 15 */ 0x2a4,
		   /* 16 */ 0x1a4,
		   /* 17 */ 0x45b,
		   /* 18 */ 0x4cb,
		   /* 19 */ 0x264,
		   /* 1a */ 0x164,
		   /* 1b */ 0x49b,
		   /* 1c */ 0x0e4,
		   /* 1d */ 0x51b,
		   /* 1e */ 0x61b,
		   /* 1f */ 0x54b,
		   /* 20 */ 0x189,
		   /* 21 */ 0x229,
		   /* 22 */ 0x129,
		   /* 23 */ 0x719,
		   /* 24 */ 0x0a9,
		   /* 25 */ 0x699,
		   /* 26 */ 0x599,
		   /* 27 */ 0x479,
		   /* 28 */ 0x069,
		   /* 29 */ 0x659,
		   /* 2a */ 0x559,
		   /* 2b */ 0x749,
		   /* 2c */ 0x4d9,
		   /* 2d */ 0x6c9,
		   /* 2e */ 0x5c9,
		   /* 2f */ 0x289,
		   /* 30 */ 0x249,
		   /* 31 */ 0x639,
		   /* 32 */ 0x539,
		   /* 33 */ 0x729,
		   /* 34 */ 0x4b9,
		   /* 35 */ 0x6a9,
		   /* 36 */ 0x5a9,
		   /* 37 */ 0x059,
		   /* 38 */ 0x0c9,
		   /* 39 */ 0x669,
		   /* 3a */ 0x569,
		   /* 3b */ 0x099,
		   /* 3c */ 0x4e9,
		   /* 3d */ 0x119,
		   /* 3e */ 0x219,
		   /* 3f */ 0x149,
		   /* 40 */ 0x185,
		   /* 41 */ 0x225,
		   /* 42 */ 0x125,
		   /* 43 */ 0x715,
		   /* 44 */ 0x0a5,
		   /* 45 */ 0x695,
		   /* 46 */ 0x595,
		   /* 47 */ 0x475,
		   /* 48 */ 0x065,
		   /* 49 */ 0x655,
		   /* 4a */ 0x555,
		   /* 4b */ 0x745,
		   /* 4c */ 0x4d5,
		   /* 4d */ 0x6c5,
		   /* 4e */ 0x5c5,
		   /* 4f */ 0x285,
		   /* 50 */ 0x245,
		   /* 51 */ 0x635,
		   /* 52 */ 0x535,
		   /* 53 */ 0x725,
		   /* 54 */ 0x4b5,
		   /* 55 */ 0x6a5,
		   /* 56 */ 0x5a5,
		   /* 57 */ 0x055,
		   /* 58 */ 0x0c5,
		   /* 59 */ 0x665,
		   /* 5a */ 0x565,
		   /* 5b */ 0x095,
		   /* 5c */ 0x4e5,
		   /* 5d */ 0x115,
		   /* 5e */ 0x215,
		   /* 5f */ 0x145,
		   /* 60 */ 0x18c,
		   /* 61 */ 0x22c,
		   /* 62 */ 0x12c,
		   /* 63 */ 0x713,
		   /* 64 */ 0x0ac,
		   /* 65 */ 0x693,
		   /* 66 */ 0x593,
		   /* 67 */ 0x473,
		   /* 68 */ 0x06c,
		   /* 69 */ 0x653,
		   /* 6a */ 0x553,
		   /* 6b */ 0x743,
		   /* 6c */ 0x4d3,
		   /* 6d */ 0x6c3,
		   /* 6e */ 0x5c3,
		   /* 6f */ 0x28c,
		   /* 70 */ 0x24c,
		   /* 71 */ 0x633,
		   /* 72 */ 0x533,
		   /* 73 */ 0x723,
		   /* 74 */ 0x4b3,
		   /* 75 */ 0x6a3,
		   /* 76 */ 0x5a3,
		   /* 77 */ 0x05c,
		   /* 78 */ 0x0cc,
		   /* 79 */ 0x663,
		   /* 7a */ 0x563,
		   /* 7b */ 0x09c,
		   /* 7c */ 0x4e3,
		   /* 7d */ 0x11c,
		   /* 7e */ 0x21c,
		   /* 7f */ 0x14c,
		   /* 80 */ 0x58d,
		   /* 81 */ 0x62d,
		   /* 82 */ 0x52d,
		   /* 83 */ 0x312,
		   /* 84 */ 0x4ad,
		   /* 85 */ 0x292,
		   /* 86 */ 0x192,
		   /* 87 */ 0x072,
		   /* 88 */ 0x46d,
		   /* 89 */ 0x252,
		   /* 8a */ 0x152,
		   /* 8b */ 0x342,
		   /* 8c */ 0x0d2,
		   /* 8d */ 0x2c2,
		   /* 8e */ 0x1c2,
		   /* 8f */ 0x68d,
		   /* 90 */ 0x64d,
		   /* 91 */ 0x232,
		   /* 92 */ 0x132,
		   /* 93 */ 0x322,
		   /* 94 */ 0x0b2,
		   /* 95 */ 0x2a2,
		   /* 96 */ 0x1a2,
		   /* 97 */ 0x45d,
		   /* 98 */ 0x4cd,
		   /* 99 */ 0x262,
		   /* 9a */ 0x162,
		   /* 9b */ 0x49d,
		   /* 9c */ 0x0e2,
		   /* 9d */ 0x51d,
		   /* 9e */ 0x61d,
		   /* 9f */ 0x54d,
		   /* a0 */ 0x18a,
		   /* a1 */ 0x22a,
		   /* a2 */ 0x12a,
		   /* a3 */ 0x71a,
		   /* a4 */ 0x0aa,
		   /* a5 */ 0x69a,
		   /* a6 */ 0x59a,
		   /* a7 */ 0x47a,
		   /* a8 */ 0x06a,
		   /* a9 */ 0x65a,
		   /* aa */ 0x55a,
		   /* ab */ 0x74a,
		   /* ac */ 0x4da,
		   /* ad */ 0x6ca,
		   /* ae */ 0x5ca,
		   /* af */ 0x28a,
		   /* b0 */ 0x24a,
		   /* b1 */ 0x63a,
		   /* b2 */ 0x53a,
		   /* b3 */ 0x72a,
		   /* b4 */ 0x4ba,
		   /* b5 */ 0x6aa,
		   /* b6 */ 0x5aa,
		   /* b7 */ 0x05a,
		   /* b8 */ 0x0ca,
		   /* b9 */ 0x66a,
		   /* ba */ 0x56a,
		   /* bb */ 0x09a,
		   /* bc */ 0x4ea,
		   /* bd */ 0x11a,
		   /* be */ 0x21a,
		   /* bf */ 0x14a,
		   /* c0 */ 0x186,
		   /* c1 */ 0x226,
		   /* c2 */ 0x126,
		   /* c3 */ 0x716,
		   /* c4 */ 0x0a6,
		   /* c5 */ 0x696,
		   /* c6 */ 0x596,
		   /* c7 */ 0x476,
		   /* c8 */ 0x066,
		   /* c9 */ 0x656,
		   /* ca */ 0x556,
		   /* cb */ 0x746,
		   /* cc */ 0x4d6,
		   /* cd */ 0x6c6,
		   /* ce */ 0x5c6,
		   /* cf */ 0x286,
		   /* d0 */ 0x246,
		   /* d1 */ 0x636,
		   /* d2 */ 0x536,
		   /* d3 */ 0x726,
		   /* d4 */ 0x4b6,
		   /* d5 */ 0x6a6,
		   /* d6 */ 0x5a6,
		   /* d7 */ 0x056,
		   /* d8 */ 0x0c6,
		   /* d9 */ 0x666,
		   /* da */ 0x566,
		   /* db */ 0x096,
		   /* dc */ 0x4e6,
		   /* dd */ 0x116,
		   /* de */ 0x216,
		   /* df */ 0x146,
		   /* e0 */ 0x58e,
		   /* e1 */ 0x62e,
		   /* e2 */ 0x52e,
		   /* e3 */ 0x311,
		   /* e4 */ 0x4ae,
		   /* e5 */ 0x291,
		   /* e6 */ 0x191,
		   /* e7 */ 0x071,
		   /* e8 */ 0x46e,
		   /* e9 */ 0x251,
		   /* ea */ 0x151,
		   /* eb */ 0x348,
		   /* ec */ 0x0d1,
		   /* ed */ 0x2c8,
		   /* ee */ 0x1c8,
		   /* ef */ 0x68e,
		   /* f0 */ 0x64e,
		   /* f1 */ 0x231,
		   /* f2 */ 0x131,
		   /* f3 */ 0x321,
		   /* f4 */ 0x0b1,
		   /* f5 */ 0x2a1,
		   /* f6 */ 0x1a1,
		   /* f7 */ 0x45e,
		   /* f8 */ 0x4ce,
		   /* f9 */ 0x261,
		   /* fa */ 0x161,
		   /* fb */ 0x49e,
		   /* fc */ 0x0e1,
		   /* fd */ 0x51e,
		   /* fe */ 0x61e,
		   /* ff */ 0x54e,
		} };
	

 
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
	
//	printAll10bwords();
	
//	printf("Sync word: %x \n", SYNC_WORD);
 
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
#define BIT_RATE 1200

double d_random(double min, double max)
{
    return min + (max - min) / RAND_MAX * rand();
}
 
int main(int argc, char * argv[])
{
	int i;
	//float amplitude = 32000;
	float amplitude = 32767/3; // 20000; // 32767/(10%amp+5%amp+100%amp)
//	float freq_Hz = 100;
	float freq_Hz = 1200; // 3000;
	
//	#define SYNC_LEN 4
//	char sync[SYNC_LEN] = {0x47, 0xcd, 0x21, 0x5d};
	long int sync = SYNC_WORD;
	int sync_bits = 31;
	
//	printf("Sync bits: %d \n", sync_bits);
	
//	#define DATA_LEN 4
//	short int b[DATA_LEN] = {0x48, 0x01, 0x80, 0x43};
	
	#define DATA_LEN 78  // 64
	#define HEADER_LEN 8  
	short int b[DATA_LEN] = {0x05,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x7E,0x03,
				0x00,0x00,0x00,0x00,0xE6,0x01,0x00,0x27,0xD1,0x02,
		        0xE5,0x40,0x04,0x18,0xE1,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
		        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	        		
		        0x00,0x00,0x00,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
		        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	short int h[HEADER_LEN] = {0x05,0x00,0x00,0x00,0x00,0x10,0x00,0x00};	        
	short int b10[DATA_LEN], h10[HEADER_LEN];

	int rd = 0;
	int nrd;	
	for (i = 0; i < DATA_LEN; i++) {
		b10[i] = (Encode_8b10b[rd][((int)b[i])] & 0x3ff);
//		nrd = getNextRd(b10[i], true);
		nrd = (Encode_8b10b[rd][((int)b[i])] >> 10) & 1;
		printf ("b[%d] = %x, b10[%d] = %x \n", i, b[i], i, b10[i]); 

		rd = nrd; // ^ nrd;
	}
	printf("\n");
	for (i = 0; i < HEADER_LEN; i++) {
		h10[i] = (Encode_8b10b[rd][((int)h[i])] & 0x3ff);
//		nrd = getNextRd(b10[i], true);
		nrd = (Encode_8b10b[rd][((int)h[i])] >> 10) & 1;
		printf ("h[%d] = %x, h10[%d] = %x \n", i, h[i], i, h10[i]); 

		rd = nrd; // ^ nrd;
	}
	
	srand((unsigned int)time(0));
	
	int samples = S_RATE / BIT_RATE;
	
	int frame_cnt = 3;
	
//	int buf_len = frame_cnt * (10 * DATA_LEN + sync_bits) * samples; // sync plus 3
//    (sync bits plus 6 data frames plus 96 check) 
	int buf_len = frame_cnt * (sync_bits + 10 * (8 + 6 * DATA_LEN + 96)) * samples; 
//	int buf_len = 128 * 8 * samples * 3;
//	int buf_len = BUF_SIZE;

   	printf("Buffer length: %d \n", buf_len);
	printf("\n\nTotal bits per frame: %d \n", buf_len/(frame_cnt * samples));
	printf("Frames: %d \n\n", frame_cnt);	
	 
//      int buffer[BUF_SIZE];
    short int buffer[buf_len];
	
	//short int meandr_value=32767;
    int phase = 1;
    int data;
    int val;
    int frames;
    int offset = 0;
    int ctr = 0;
    
    for (frames = 0; frames < frame_cnt; frames++) 
    {
 
 	for (i = 1; i <= sync_bits * samples; i++)
	{
 		buffer[ctr++] = (int)(amplitude * phase * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 			
		if ( (i % samples) == 0) {
  			int bit = sync_bits - i/samples + 1;
  			val = sync;
			data = val & 1 << (bit - 1);	
		    printf ("%d i: %d new frame %d sync bit %d = %d \n",
		    		 ctr/samples, i, frames, bit, (data > 0) );
			if (data == 0)  {
				phase *= -1;
			}
		}
	}
	
	for (i = 1; i <= (10 * 8 * samples); i++)  // header
	{
		buffer[ctr++] = (int)(amplitude * phase * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 
		if ( (i % samples) == 0) {
			int symbol = (int)((i - 1)/ (samples * 10));
			int bit = 10 - (i - symbol * samples * 10) / samples + 1;	
//			int bit = (i - symbol * samples * 10) / samples;	
			val = h10[symbol];
			data = val & 1 << (bit - 1);	
			printf ("%d i: %d new frame %d h10[%d] = %x bit %d = %d \n",
		    		 ctr/samples, i, frames, symbol, val, bit, (data > 0) );
			if (data == 0)  {
				phase *= -1;
			}
		}
	 }

  for (int j = 1; j <= 6; j++) {  // six payloads in a row
	for (i = 1; i <= (10 * DATA_LEN * samples); i++)
	{
		buffer[ctr++] = (int)(amplitude * phase * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 
		if ( (i % samples) == 0) {
			int symbol = (int)((i - 1)/ (samples * 10));
			int bit = 10 - (i - symbol * samples * 10) / samples + 1;	
//			int bit = (i - symbol * samples * 10) / samples;	
			val = b10[symbol];
			data = val & 1 << (bit - 1);	
			printf ("%d i: %d new frame %d b10[%d] = %x bit %d = %d \n",
		    		 ctr/samples, i, frames, symbol, val, bit, (data > 0) );
			if (data == 0)  {
				phase *= -1;
			}
		}
	 }
   }
   
   	for (i = 1; i <= (10 * 96 * samples); i++)  // zero out FCS
	{
		buffer[ctr++] = (int)(amplitude * phase * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 
 		if ( (i % samples) == 0) {
// 				printf ("%d i: %d new frame %d zero\n",
//		    		ctr/samples, i, frames);
// 				phase *= -1;        // all zeros
			int symbol = (int)((i - 1)/ (samples * 10));
			int bit = 10 - (i - symbol * samples * 10) / samples + 1;	
//			int bit = (i - symbol * samples * 10) / samples;	
			val = 0x18b;  // zero
			data = val & 1 << (bit - 1);	
			printf ("%d i: %d new frame %d zero %d = %x bit %d = %d \n",
		    		 ctr/samples, i, frames, symbol, val, bit, (data > 0) );
			if (data == 0)  {
				phase *= -1;
			}		
 		}
   }
   
	}
	write_wav("make_wav2.wav", buf_len, buffer, S_RATE);
 
	return 0;
}

//package decoder;

/**
 * 
 * FOX 1 Telemetry Decoder
 * @author chris.e.thompson g0kla/ac2cz
 *
 * Copyright (C) 2015 amsat.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General  License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General  License for more details.
 *
 * You should have received a copy of the GNU General  License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 * Static variables and methods to encode and decode 8b10b
 * 
 *
 */
// class Code8b10b {
//#include <stdio.h>


static int getNextRd(int word, int flip);
static int getRdSense10b(int word, int flip);
static char decode(int word, int flip);
static void printAll10bwords();
static int encode(int word);	 
	
	/**
	 * Given a 10 bit word, look it up in the Encode_8b10b matrix and return the value
	 * @param word - the 10 bit word
	 * @return - the value returned 
	 * @throws LookupException 
	 * 
	 */
	 static char decode(int word, int flip) { // throws LookupException {
		if (flip)   {
			 printf("%x flipped ", word);
			 word = ~word & 0x3ff;
			 printf("is %x \n", word);
		}
		for (int rd=0; rd<2; rd++)
			for (int i=0; i<256; i++) {
				int testValue = Encode_8b10b[rd][i];
				if ((testValue & 0x3ff) == word) {				
					return (char)i; // return the position in the lookup table where we found the code word
				}
			}
		return 0;
//		throw new LookupException();
	}
	
	 static void printAll10bwords() {
		for (int rd=0; rd<2; rd++) {
			printf("************* RD: %d \n", rd);
			for (int i=0; i<256; i++) {
				int testValue = Encode_8b10b[rd][i];
				printf("Encode_8b10b[%x][%x]: %x ", rd, i, testValue);
				int nextRd = getNextRd(testValue, false);
				printf(" Next RD: %d \n", nextRd);
				if (nextRd == -99) {
					nextRd = getNextRd(testValue, true);
					printf(" Flipped Next RD: %d \n", nextRd);
				}	
//				testValue = testValue & 0x3ff;				
//				int[] bit10b = FoxBitStrtestValue);
//				for (int j=0; j < bit10b.length; j++)
//					if (bit10b[j]) printf(1 + " "); else printf(0 + " ");
//				printf(" Next rd: %d", nextRd);
			}
		}
			
	}
	
	/**
	 * For test purposes.  Given a chars, return the 10b encoded bits.
	 * Always the -1 rd parity
	 * @param word
	 * @return
	 */
	 static int encode(int word) {
		int word10b = Encode_8b10b[0][word];
		return word10b;
	}

	/**
	 * Returns the Running Disparity to be used for the next call to the encoder given this word
	 * @param word
	 * @param flip
	 * @return
	 */
	 static int getNextRd(int word, int flip) {
		if (flip) word = ~word & 0x3ff;
		for (int rd=0; rd<2; rd++)
			for (int i=0; i<256; i++) {
				int testValue = Encode_8b10b[rd][i];
//				printf("word: %x  testValue: %x  && 0x3ff %x  == %d \n",
//					 word, testValue, testValue & 0x3ff, (testValue & 0x3ff) == word);
				if ((testValue & 0x3ff) == word) {				
//				if ((testValue) == word) {				
					int state = (testValue >> 10) & 1;
					return state;
				}
			}
		return -99; // error state - we did not find the RD sense
		
		
		
		
	}
	
	/**
	 * Returns the Running Disparity (RD) used to encode the given word by finding it in the look up table
	 * @param word
	 * @return
	 */
	 static int getRdSense10b(int word, int flip) {
		if (flip) word = ~word & 0x3ff;
		for (int rd=0; rd<2; rd++)
			for (int i=0; i<256; i++) {
				int testValue = Encode_8b10b[rd][i];
				if ((testValue & 0x3ff) == word) {				
					return rd;
				}
			}
		return -99; // error state - we did not find the RD sense
	}
	
//}
