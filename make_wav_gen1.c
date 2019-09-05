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
 
 /*
 * TelemEncoding.h
 *
 *  Created on: Feb 3, 2014
 *      Author: fox
 */
 
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "make_wav.h"

#define false 0
#define true 1

static int getNextRd(int word, int flip);
static int getRdSense10b(int word, int flip);
static char decode(int word, int flip);
static void printAll10bwords();
static int encode(int word);
static int twosToInt(int val,int len);
static int encodeB(short int  *b, int index, int val);
static int encodeA(short int  *b, int index, int val);

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
	

/*
 * TelemEncoding.c
 *
   Fox-1 telemetry encoder
   January 2014 Phil Karn KA9Q

   This file has two external functions:
      void update_rs(unsigned char parity[32],unsigned char data);
      int encode_8b10b(int *state,int data).

   update_rs() is the Reed-Solomon encoder. Its first argument is the 32-byte
   encoder shift register, the second is the 8-bit data byte being encoded. It updates
   the shift register in place and returns void. At the end of each frame, it contains
   the parities ready for transmission, starting with parity[0].
   Be sure to zero this array before each new frame!

   encode_8b10b() is the 8b10b encoder. Its first argument is a pointer to a single integer
   with the 1-bit encoder state (the current run disparity, or RD). Initialize it to 0
   JUST ONCE at startup (not between frames).
   The second argument is the data byte being encoded. It updates the state and returns
   an integer containing the 10-bit encoded word, right justified.
   Transmit this word from left to right.

   The data argument is an int so it can hold the special value -1 to indicate end of frame;
   it generates the 8b10b control word K.28.5, which is used as an inter-frame flag.

   Some assert() calls are made to verify legality of arguments. These can be turned off in
   production code.


   sample frame transmission code:

   unsigned char data[64]; // Data block to be sent
   unsigned char parity[32]; // RS parities
   void transmit_word(int);  // User provided transmit function: 10 bits of data in bits 9....0
   int state,i;

   state = 0; // Only once at startup, not between frames
   memset(parity,0,sizeof(parity); // Do this before every frame
   // Transmit the data, updating the RS encoder
   for(i=0;i<64;i++){
     update_rs(parity,data[i]);
     transmit_word(encode_8b10b(&state,data[i]);
   }
   // Transmit the RS parities
   for(i=0;i<32;i++)
     transmit_word(encode_8b10b(&state,parity[i]);

   transmit_word(encode_8b10b(&state,-1); // Transmit end-of-frame flag
*/


//#include "FreeRTOS.h"
//#include "task.h"
#include <string.h>
//#include "Fox.h"
//#include "TelemEncoding.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#define NN (0xff) // Frame size in symbols
#define A0 (NN)   // special value for log(0)


// GF Antilog lookup table table
static unsigned char CCSDS_alpha_to[NN+1] = {
0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x87,0x89,0x95,0xad,0xdd,0x3d,0x7a,0xf4,
0x6f,0xde,0x3b,0x76,0xec,0x5f,0xbe,0xfb,0x71,0xe2,0x43,0x86,0x8b,0x91,0xa5,0xcd,
0x1d,0x3a,0x74,0xe8,0x57,0xae,0xdb,0x31,0x62,0xc4,0x0f,0x1e,0x3c,0x78,0xf0,0x67,
0xce,0x1b,0x36,0x6c,0xd8,0x37,0x6e,0xdc,0x3f,0x7e,0xfc,0x7f,0xfe,0x7b,0xf6,0x6b,
0xd6,0x2b,0x56,0xac,0xdf,0x39,0x72,0xe4,0x4f,0x9e,0xbb,0xf1,0x65,0xca,0x13,0x26,
0x4c,0x98,0xb7,0xe9,0x55,0xaa,0xd3,0x21,0x42,0x84,0x8f,0x99,0xb5,0xed,0x5d,0xba,
0xf3,0x61,0xc2,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x07,0x0e,0x1c,0x38,0x70,0xe0,
0x47,0x8e,0x9b,0xb1,0xe5,0x4d,0x9a,0xb3,0xe1,0x45,0x8a,0x93,0xa1,0xc5,0x0d,0x1a,
0x34,0x68,0xd0,0x27,0x4e,0x9c,0xbf,0xf9,0x75,0xea,0x53,0xa6,0xcb,0x11,0x22,0x44,
0x88,0x97,0xa9,0xd5,0x2d,0x5a,0xb4,0xef,0x59,0xb2,0xe3,0x41,0x82,0x83,0x81,0x85,
0x8d,0x9d,0xbd,0xfd,0x7d,0xfa,0x73,0xe6,0x4b,0x96,0xab,0xd1,0x25,0x4a,0x94,0xaf,
0xd9,0x35,0x6a,0xd4,0x2f,0x5e,0xbc,0xff,0x79,0xf2,0x63,0xc6,0x0b,0x16,0x2c,0x58,
0xb0,0xe7,0x49,0x92,0xa3,0xc1,0x05,0x0a,0x14,0x28,0x50,0xa0,0xc7,0x09,0x12,0x24,
0x48,0x90,0xa7,0xc9,0x15,0x2a,0x54,0xa8,0xd7,0x29,0x52,0xa4,0xcf,0x19,0x32,0x64,
0xc8,0x17,0x2e,0x5c,0xb8,0xf7,0x69,0xd2,0x23,0x46,0x8c,0x9f,0xb9,0xf5,0x6d,0xda,
0x33,0x66,0xcc,0x1f,0x3e,0x7c,0xf8,0x77,0xee,0x5b,0xb6,0xeb,0x51,0xa2,0xc3,0x00,
};

// GF log lookup table. Special value represents log(0)
static unsigned char CCSDS_index_of[NN+1] = {
 A0,  0,  1, 99,  2,198,100,106,  3,205,199,188,101,126,107, 42,
  4,141,206, 78,200,212,189,225,102,221,127, 49,108, 32, 43,243,
  5, 87,142,232,207,172, 79,131,201,217,213, 65,190,148,226,180,
103, 39,222,240,128,177, 50, 53,109, 69, 33, 18, 44, 13,244, 56,
  6,155, 88, 26,143,121,233,112,208,194,173,168, 80,117,132, 72,
202,252,218,138,214, 84, 66, 36,191,152,149,249,227, 94,181, 21,
104, 97, 40,186,223, 76,241, 47,129,230,178, 63, 51,238, 54, 16,
110, 24, 70,166, 34,136, 19,247, 45,184, 14, 61,245,164, 57, 59,
  7,158,156,157, 89,159, 27,  8,144,  9,122, 28,234,160,113, 90,
209, 29,195,123,174, 10,169,145, 81, 91,118,114,133,161, 73,235,
203,124,253,196,219, 30,139,210,215,146, 85,170, 67, 11, 37,175,
192,115,153,119,150, 92,250, 82,228,236, 95, 74,182,162, 22,134,
105,197, 98,254, 41,125,187,204,224,211, 77,140,242, 31, 48,220,
130,171,231, 86,179,147, 64,216, 52,176,239, 38, 55, 12, 17, 68,
111,120, 25,154, 71,116,167,193, 35, 83,137,251, 20, 93,248,151,
 46, 75,185, 96, 15,237, 62,229,246,135,165, 23, 58,163, 60,183,
};

// Only half the coefficients are given here because the
// generator polynomial is palindromic; G0 = G32, G1 = G31, etc.
// Only G16 is unique
static unsigned char CCSDS_poly[] = {
  0,249,  59, 66,  4,  43,126,251, 97,  30,   3,213, 50, 66,170,   5,
  24,
};


static inline int modnn(int x){
  while (x >= NN) {
    x -= NN;
    x = (x >> 8) + (x & NN);
  }
  return x;
}


// Update Reed-Solomon encoder
// parity -> 32-byte reed-solomon encoder state; clear this to zero before each frame
void update_rs(
   unsigned char parity[32], // 32-byte encoder state; zero before each frame
   unsigned char c)          // Current data byte to update
{
  unsigned char feedback;
  int j,t;

  assert(parity != NULL);
  feedback = CCSDS_index_of[c ^ parity[0]];
  if(feedback != A0){ // only if feedback is non-zero
    // Take advantage of palindromic polynomial to halve the multiplies
    // Do G1...G15, which is the same as G17...G31
    for(j=1;j<NP/2;j++){
      t = CCSDS_alpha_to[modnn(feedback + CCSDS_poly[j])];
      parity[j] ^= t;
      parity[NP-j] ^= t;
    }
    // Do G16, which is used in only parity[16]
    t = CCSDS_alpha_to[modnn(feedback + CCSDS_poly[j])];
    parity[j] ^= t;
  }
  // shift left
  memmove(&parity[0],&parity[1],NP-1);
  // G0 is 1 in alpha form, 0 in index form; don't need to multiply by it
  parity[NP-1] = CCSDS_alpha_to[feedback];
  //taskYIELD();
}

#define SYNC  (0x0fa) // K.28.5, RD=-1


// 8b10b encoder
// state -> 1-bit state (0 -> RD=-1, 1 -> RD=+1), updates in place
// data is 0-255 for user data, -1 for end of frame flag
// Returns 10-bit channel word, right justified. Transmit this word left-to-right.
int encode_8b10b(
    int32_t *state, // pointer to encoder state (run disparity, RD)
    int32_t data){

  uint32_t w,st;

  assert(state != NULL);
  st = *state & 0x1;
  if(data < 0){
    // Encode end-of-frame sync. It has 6 1-bits so the RD always toggles
    w = st ? ~SYNC : SYNC;
    st = !st;
  } else {
    w = Encode_8b10b[st][data & 0xff];
    st = (w >> 10) & 1; // Extract new RD
  }
  *state = st;
  return w & 0x3ff;
}


 
 
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
/*
// BPSK Settings
#define BIT_RATE	1200 // 200 for DUV
#define DUV	0 // 1 for DUV
#define RS_FRAMES 3 // 3 frames for BPSK, 1 for DUV
#define PAYLOADS 6 // 1 for DUV
#define DATA_LEN 78  // 56 for DUV  
#define RS_FRAME_LEN 159  // 64 for DUV
#define SYNC_BITS 31  // 10 for DUV
#define SYNC_WORD 0b1000111110011010010000101011101 // 0b0011111010 for DUV
#define HEADER_LEN 8  // 6 for DUV
*/
// DUV Settings
#define BIT_RATE 200 
#define DUV	1
#define RS_FRAMES 1
#define PAYLOADS 1
#define RS_FRAME_LEN 64
#define HEADER_LEN 6
#define DATA_LEN 58
#define SYNC_BITS 10
#define SYNC_WORD 0b0011111010


double d_random(double min, double max)
{
    return min + (max - min) / RAND_MAX * rand();
}



#define PARITY_LEN 32

float amplitude = 32767/3; // 20000; // 32767/(10%amp+5%amp+100%amp)
float freq_Hz = 3000;  // 1200
	
int smaller;
int flip_ctr = 0;
int phase = 1;
int ctr = 0;
void write_to_buffer(int i, int symbol, int val);	
void write_wave();
#define SAMPLES (S_RATE / BIT_RATE)
#define FRAME_CNT 5	

//#define BUF_LEN (FRAME_CNT * (SYNC_BITS + 10 * (8 + 6 * DATA_LEN + 96)) * SAMPLES)     
#define BUF_LEN (FRAME_CNT * (SYNC_BITS + 10 * (HEADER_LEN + RS_FRAMES * (RS_FRAME_LEN + PARITY_LEN))) * SAMPLES)    
short int buffer[BUF_LEN];
short int data10[8 + RS_FRAMES * (RS_FRAME_LEN + PARITY_LEN)];
short int data8[8 + RS_FRAMES * (RS_FRAME_LEN + PARITY_LEN)]; 
int main(int argc, char * argv[])
{
	int i;
	//float amplitude = 32000;
//	#define SYNC_LEN 4
//	char sync[SYNC_LEN] = {0x47, 0xcd, 0x21, 0x5d};
	long int sync = SYNC_WORD;

	smaller = S_RATE/(2 * freq_Hz);
	
	short int b[DATA_LEN] = {0x00,0x7E,0x03,
				0x00,0x00,0x00,0x00,0xE6,0x01,0x00,0x27,0xD1,0x02,
		        0xE5,0x40,0x04,0x18,0xE1,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
		        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	        		
		        0x00,0x00,0x00,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
		        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		         
	short int h[HEADER_LEN] = {0x05,0x00,0x00,0x00,0x00,0x10,0x00,0x00};	
	        
	short int b10[DATA_LEN], h10[HEADER_LEN];
//	short int rs_frame[3][RS_FRAME_LEN];
	short int rs_frame[RS_FRAMES][223];
//	memset(rs_frame,0,sizeof(rs_frame));
//    uint8_t parities[3][PARITY_LEN],inputByte;
    unsigned char parities[RS_FRAMES][PARITY_LEN],inputByte;
// 	memset(parities,0,sizeof(parities));
 	
  int id = 1, frm_type = 0x01, TxTemp = 0, IHUcpuTemp = 0; 
  int batt_a_v = 0, batt_b_v = 0, batt_c_v = 8.95 * 100, battCurr = 48.6 * 10;
  int posXv = 2.95 * 100, negXv = 0.45 * 100, posYv = 2.3 * 100, negYv = 0.68 * 100, posZv = 2.8 * 100, negZv = 0.78 * 100;
  int head_offset = 6;
	
  b[0] = b[0] | (id & 0x07);  // 3 bits
  b[5] = b[5] | (frm_type << 4);  

  encodeA(b, 0 + head_offset, batt_a_v);
  encodeB(b, 1 + head_offset, batt_b_v);
  encodeA(b, 3 + head_offset, batt_c_v);
  encodeA(b, 9 + head_offset, battCurr);
  encodeA(b, 12 + head_offset,posXv);	
  encodeB(b, 13 + head_offset,negXv);	
  encodeA(b, 15 + head_offset,posYv);	
  encodeB(b, 16 + head_offset,negYv);	
  encodeA(b, 18 + head_offset,posZv);	
  encodeB(b, 19 + head_offset,negZv);		

    for (int frames = 0; frames < FRAME_CNT; frames++) 
    {
    	memset(rs_frame,0,sizeof(rs_frame));
 		memset(parities,0,sizeof(parities));

		// increment id
//		h[0] += 1;
     	// increment uptime
    	h[3] += 1;
//    	h[5] += 1;
//    	h[7] += 1;
    	
    	// battery voltage increment
//    	b[1] += 1;
//    	b[3] += 1;  // in DUV this increments battAtemp
	 
	 batt_c_v += 10;
	 battCurr -= 10;
	 encodeA(b, 3 + head_offset, batt_c_v);
 	 encodeA(b, 9 + head_offset, battCurr);
       
    	printf("Frame: %d Id: %d Uptime: %x Battery: %x \n", frames, h[0], h[3], b[3]);

	int ctr1 = 0;
	int frame_rs = 0;	
	for (int i = 0; i < HEADER_LEN; i++)  // header
	{
         rs_frame[frame_rs][ctr1++] = h[i];
		 update_rs(parities[frame_rs],h[i]);
         printf("\n%d rs_frame[%d][%d] = %x \n", ctr1, frame_rs, ctr1 - 1, h[i]);
    }   
       
    int pos = 0;
    for (int j = 0; j < PAYLOADS; j++)   // six payloads in a row for BPSK
    {   	
    	for (int k = 0; k < DATA_LEN; k++)
    	{ 
    		 frame_rs = (int) (ctr1 / RS_FRAME_LEN);
    		 pos = ctr1++ - frame_rs * RS_FRAME_LEN;
             rs_frame[frame_rs][pos] = b[k];
		     update_rs(parities[frame_rs],b[k]);
             printf("%d rs_frame[%d][%d] = %x \n", ctr1, frame_rs, pos, b[k]);
    	}
    }	
    	printf("Parities ");
		for (int m = 0; m < PARITY_LEN; m++) {
		 	printf("%d ", parities[0][m]);
		}
		printf("\n");
 
  	int ctr2 = 0;    
 	memset(data10,0,sizeof(data10));
 	memset(data8,0,sizeof(data8));
    
    for (i = 0; i < RS_FRAME_LEN; i++) 
	{
		for (int j  = 0; j < RS_FRAMES ; j++)
		{
			if (!((i == (RS_FRAME_LEN - 1)) && (j == 2))) // skip last one for BPSK
			{
				data8[ctr2++] = rs_frame[j][i];
				printf ("data8[%d] = rs_frame[%d][%d] = %x \n",
					ctr2 - 1, j, i, rs_frame[j][i]);
			}
		}	
	}    
     
  	ctr2 = 0; 
  	int rd = 0;
	int nrd;	
 
    for (i = 0; i < DATA_LEN * PAYLOADS + HEADER_LEN; i++) // 476 for BPSK
	{
				data10[ctr2] = (Encode_8b10b[rd][((int)data8[ctr2])] & 0x3ff);
				nrd = (Encode_8b10b[rd][((int)data8[ctr2])] >> 10) & 1;
				printf ("data10[%d] = encoded data8[%d] = %x \n",
				 	ctr2, ctr2, data10[ctr2]); 

				rd = nrd; // ^ nrd;
				ctr2++;
	}
  
    for (i = 0; i < PARITY_LEN; i++) 
	{
		for (int j  = 0; j < RS_FRAMES; j++)
		{
			data10[ctr2++] = (Encode_8b10b[rd][((int)parities[j][i])] & 0x3ff);
			nrd = (Encode_8b10b[rd][((int)parities[j][i])] >> 10) & 1;
			printf ("data10[%d] = encoded parities[%d][%d] = %x \n",
				 ctr2 - 1, j, i, data10[ctr2 - 1]); 

			rd = nrd; 
		}	
	}
     
/*   
   	printf("Buffer length: %d \n", BUF_LEN);
	printf("\n\nTotal bits per frame: %d \n", BUF_LEN/(FRAME_CNT * SAMPLES));
	printf("Frame Count: %d \n\n", FRAME_CNT);	
*/
	 
    int data;
    int val;
    int offset = 0;
      
 	for (i = 1; i <= SYNC_BITS * SAMPLES; i++)
	{
		write_wave(ctr);	
		if ( (i % SAMPLES) == 0) {
  			int bit = SYNC_BITS - i/SAMPLES + 1;
  			val = sync;
			data = val & 1 << (bit - 1);	
		    printf ("%d i: %d new frame %d sync bit %d = %d \n",
		    		 ctr/SAMPLES, i, frames, bit, (data > 0) );
			if (DUV)
			{
				phase = ((data != 0) * 2) - 1; 
				printf("Sending a %d\n", phase);
			}
			else
			{
				if (data == 0)  {
					phase *= -1;
					if ( (ctr - smaller) > 0)
					{
						for (int j = 1; j <= smaller; j++)
				     		buffer[ctr - j] = buffer[ctr - j] * 0.4;
					}
					flip_ctr = ctr;
				}
			}
		}
	}

	for (i = 1; 
	  i <= (10 * (HEADER_LEN + DATA_LEN * PAYLOADS + RS_FRAMES * PARITY_LEN) * SAMPLES); i++) // 572   
	{
		write_wave(ctr);
		if ( (i % SAMPLES) == 0) {
			int symbol = (int)((i - 1)/ (SAMPLES * 10));
			int bit = 10 - (i - symbol * SAMPLES * 10) / SAMPLES + 1;	
			val = data10[symbol];
			data = val & 1 << (bit - 1);	
			printf ("%d i: %d new frame %d data10[%d] = %x bit %d = %d \n",
		    		 ctr/SAMPLES, i, frames, symbol, val, bit, (data > 0) );
		    if (DUV)
			{
				phase = ((data != 0) * 2) - 1; 
				printf("Sending a %d\n", phase);
			}
			else
			{	 
				if (data == 0)  {
					phase *= -1;
					if ( (ctr - smaller) > 0)
					{
						for (int j = 1; j <= smaller; j ++)
				    	 	buffer[ctr - j] = buffer[ctr - j] * 0.4;
					}
					flip_ctr = ctr;
				}
			}	
		}
	 }   
	}
	write_wav("make_both2.wav", BUF_LEN, buffer, S_RATE);
 
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

void write_wave(int i)
{
		if (DUV)
		{
//			if ((ctr - flip_ctr) < smaller)
//				buffer[ctr++] = 0.1 * phase * (ctr - flip_ctr) / smaller;
//			else
				buffer[ctr++] = 0.25 * amplitude * phase;			
		}
		else
		{
			if ((ctr - flip_ctr) < smaller)
  		 		buffer[ctr++] = (int)(amplitude * 0.4 * phase * 
  		 								sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 					
 			else
 		 		buffer[ctr++] = (int)(amplitude * phase * 		
 		 								sin((float)(2*M_PI*i*freq_Hz/S_RATE)));
 		 } 			
//		printf("%d %d \n", i, buffer[ctr - 1]);

}

void write_to_buffer(int i, int symbol, int val)
{
//		if ((ctr - flip_ctr) < smaller)
//  		 buffer[ctr++] = (int)(amplitude * 0.4 * phase * 
//  		 								sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 					
// 		else
// 		 buffer[ctr++] = (int)(amplitude * phase * sin((float)(2*M_PI*i*freq_Hz/S_RATE))); 			
//		if ( (i % SAMPLES) == 0) {
//			int symbol = (int)((i - 1)/ (SAMPLES * 10));
			int bit = 10 - (i - symbol * SAMPLES * 10) / SAMPLES + 1;	
//			int bit = (i - symbol * SAMPLES * 10) / SAMPLES;	
//			val = b10[symbol];
			int data = val & 1 << (bit - 1);	
			printf ("%d i: %d val = %x bit %d = %d \n",
		    		 ctr/SAMPLES, i, val, bit, (data > 0) );
			if (data == 0)  {
				phase *= -1;
				if ( (ctr - smaller) > 0)
				{
					for (int j = 1; j <= smaller; j++)
				     	buffer[ctr - j] = buffer[ctr - j] * 0.4;
				}
				flip_ctr = ctr;
			} 
//		}
}

 int getNextRd(int word, int flip);
 int getRdSense10b(int word, int flip);
 char decode(int word, int flip);
 void printAll10bwords();
 int encode(int word);	 
	
	/**
	 * Given a 10 bit word, look it up in the Encode_8b10b matrix and return the value
	 * @param word - the 10 bit word
	 * @return - the value returned 
	 * @throws LookupException 
	 * 
	 */
	 
	  char decode(int word, int flip) { // throws LookupException {
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
	
	  void printAll10bwords() {
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
	  int encode(int word) {
		int word10b = Encode_8b10b[0][word];
		return word10b;
	}

	/**
	 * Returns the Running Disparity to be used for the next call to the encoder given this word
	 * @param word
	 * @param flip
	 * @return
	 */
	  int getNextRd(int word, int flip) {
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
	  int getRdSense10b(int word, int flip) {
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
	
int encodeA(short int  *b, int index, int val) {
//    printf("Encoding A\n");
    b[index] = (val & 0xff);
    b[index + 1] = b[index + 1] | (val >> 8);
    return 0;	
}

int encodeB(short int  *b, int index, int val) {
//    printf("Encoding B\n");
    b[index] = b[index]  |  (val << 4);
    b[index + 1] = b[index + 1]  |  ((val >> 4 ) & 0xff);
    return 0;	
}

int twosToInt(int val,int len) {   // Convert twos compliment to integer
// from https://www.raspberrypi.org/forums/viewtopic.php?t=55815
	
      if(val & (1 << (len - 1)))
         val = val - (1 << len);

      return(val);
}
