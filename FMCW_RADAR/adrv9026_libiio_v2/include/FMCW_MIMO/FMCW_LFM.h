#ifndef _FMCW_LFM_H_
#define _FMCW_LFM_H_

// MIMO雷达体制下的雷达工作模式
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fftw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
//
#include "FMCW_MIMO_Algorithm.h"

struct MIMO_LFM_conf{
	float K;    // 调频斜率
	float fs;   // 采样率
	float B;    // 带宽
	float PRI;  // 脉冲重复间隔
	float tp;   // 采样率
	float PulseNum;
  float move;
	unsigned int PulseLength;
	unsigned int SignalLength;
  bool _use_half_band_;
  Eigen::ArrayXXcf FMCW_wave;
  Eigen::ArrayXXcf FMCW_wave_conj;
  Eigen::ArrayXXcf FFT_FMCW;
};

// 初始化LFM信号配置
void MIMO_LFM_configure(MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, bool ec[4], MIMO_LFM_conf & conf);

#endif