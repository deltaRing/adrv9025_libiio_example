#ifndef _LFM_H_
#define _LFM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// signal define and convert into bit 14
#define _2_14_ 16383
#define _PI    3.1415926535
#define BLOCK_SIZE (int)(1024)

// inoutput 1: buffer_i
// inoutput 2: buffer_q
// input 3: bandwidth B
// input 4: sample rate
void cosine_signal(int16_t * buffer_i, int16_t * buffer_q, float fc, float fs);

void constant_signal(int16_t * buffer_i, int16_t * buffer_q);

void stamp_signal(int16_t * buffer_i, int16_t * buffer_q, float start, float end);

// inoutput 1: buffer_i
// inoutput 2: buffer_q
// input 3: bandwidth B
// input 4: sample rate
void LFM_signal(int16_t * buffer_i, int16_t * buffer_q, float B, float tc, float fs);

// By default the TX RF bandwidth is 450000000
// By default the TX sample frequency 245760000
// By default the RX RF bandwidth is 200000000
// By default the RX sample frequency 245760000

#endif
