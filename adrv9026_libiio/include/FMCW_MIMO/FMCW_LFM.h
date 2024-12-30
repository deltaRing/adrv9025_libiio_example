#ifndef _FMCW_LFM_H_
#define _FMCW_LFM_H_

// MIMO雷达体制下的雷达工作模式
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fftw3.h>

#include <cmath>
#include <iostream>
//
struct MIMO_LFM_conf{
	float K;    // 调频斜率
	float fs;   // 采样率
	float B;    // 带宽
  float info; // 信息脉冲 + 间隔
	float PRI;  // 脉冲重复间隔
	float tp;   // 采样率
	float PulseNum;
	unsigned int PulseLength;
	unsigned int SignalLength;
};

// 初始化LFM信号配置
void MIMO_LFM_configure(MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf conf);

#endif