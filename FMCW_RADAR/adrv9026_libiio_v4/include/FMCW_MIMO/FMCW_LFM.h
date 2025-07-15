#ifndef _FMCW_LFM_H_
#define _FMCW_LFM_H_

// MIMO Radar Define
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fftw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
//
#include "FMCW_MIMO_Algorithm.h"

struct MIMO_LFM_conf{
	float K;    // slope
	float fs;   // sample rate
	float B;    // band width
	float PRI;  // PRI
	float tp;   // valid pulse length
	float PulseNum;
  float move;
	unsigned int PulseLength;
	unsigned int SignalLength;
  bool _use_half_band_;
  Eigen::ArrayXXcf FMCW_wave;
  Eigen::ArrayXXcf FMCW_wave_conj;
  Eigen::ArrayXXcf FFT_FMCW;
};

// Default Parameters
void MIMO_LFM_configure(MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, bool ec[4], MIMO_LFM_conf & conf);

#endif