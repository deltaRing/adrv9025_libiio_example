#include "../../include/Utilize/test_signal.h"

// inoutput 1: buffer_i
// inoutput 2: buffer_q
// input 3: bandwidth B
// input 4: sample rate
void cosine_signal(int16_t * buffer_i, int16_t * buffer_q, float fc, float fs){
	if (buffer_i == NULL || buffer_q == NULL){
		printf("cosine signal error: buffer i or buffer q is Null\n");
		return;
	}
 
  float t = 1 / fs;
  for (int ii = 0; ii < BLOCK_SIZE; ii++){
    buffer_i[ii] = (int16_t)(cos(2 * _PI * fc * t * ii) * _2_14_ * 0.5);
    buffer_q[ii] = (int16_t)(sin(2 * _PI * fc * t * ii) * _2_14_ * 0.5);
  }
}

void constant_signal(int16_t * buffer_i, int16_t * buffer_q){
	if (buffer_i == NULL || buffer_q == NULL){
		printf("cosine signal error: buffer i or buffer q is Null\n");
		return;
	}
 
  for (int ii = 0; ii < BLOCK_SIZE; ii++){
    buffer_i[ii] = (int16_t)(1 * _2_14_);
    buffer_q[ii] = (int16_t)(1 * _2_14_);
  }
}

void stamp_signal(int16_t * buffer_i, int16_t * buffer_q, float start, float end){
  if (buffer_i == NULL || buffer_q == NULL){
		printf("LFM signal error: buffer i or buffer q is Null\n");
		return;
	}
 
   // normalize the start value and end value
   float delta = (end - start) / BLOCK_SIZE;
   for (int ii = 0; ii < BLOCK_SIZE; ii++){
     buffer_i[ii] = (int16_t)((start + ii * delta) * _2_14_ * 0.5);
     buffer_q[ii] = (int16_t)((start + ii * delta) * _2_14_ * 0.5);
   }
}

// inoutput 1: buffer_i
// inoutput 2: buffer_q
// input 3: bandwidth B
// input 4: tc LFM信号持续长度
// input 5: sample rate by default is 
void LFM_signal(int16_t * buffer_i, int16_t * buffer_q, float B, float tc, float fs){
	if (buffer_i == NULL || buffer_q == NULL){
		printf("LFM signal error: buffer i or buffer q is Null\n");
		return;
	}
 
  float t = 1 / fs, k = B / tc; // frequency modulation rate
  for (int ii = 0; ii < BLOCK_SIZE; ii++){
    if (ii * t < tc){
      buffer_i[ii] = (int16_t)(cos(k * _PI * t * t * ii * ii) * _2_14_);
      buffer_q[ii] = (int16_t)(sin(k * _PI * t * t * ii * ii) * _2_14_);
    }
    else{
      buffer_i[ii] = 0;
      buffer_q[ii] = 0;
    }
  }
}

