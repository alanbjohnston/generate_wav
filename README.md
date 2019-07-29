 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 * 
 * Edited by Dolin Sergey. dlinyj@gmail.com
 * April 11 12:58 2014
 
 
 
 git clone  ...
 
 cd generate_wav
 
 gcc -o make_wav make_wav.c -lm
 
 ./make_wav
 
 aplay test.wav
 
 Playing WAVE 'test.wav' : Signed 16 bit Little Endian, Rate 44100 Hz, Mono
 
