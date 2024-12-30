#ifndef _LFM_H_
#define _LFM_H_

// Eigen 相关
#include <Eigen/Core>
#include <Eigen/Dense>
// FFT 相关
#include <fftw3.h>
// Cmath
#include <iostream>
#include <cmath>

#define _LFM_USE_ 1
#define _PhaseCode_ 0
#define _PI_ 3.1415926535

struct LFM_conf {
	float K;    // 调频斜率
	float fs;   // 采样率
	float B;    // 带宽
	float PRI;  // 脉冲重复间隔
	float tp;   // 采样率
	unsigned int PulseNum;
	unsigned int PulseLength;
	unsigned int SignalLength;
};

// 初始化LFM信号配置
void LFM_configure(LFM_conf& conf);

// 生成LFM信号
Eigen::ArrayXcf LFM_waveform(float fs, float B, float PRI, float tp);
// 生成匹配滤波器
Eigen::ArrayXcf LFM_matchfilter(Eigen::ArrayXcf signal);
// 脉冲压缩
Eigen::ArrayXcf LFM_pulsecompression(Eigen::ArrayXcf Echo, 
									Eigen::ArrayXcf matchfilter,
									unsigned int pulse_num,
									unsigned int pulse_length);
// MTI 动目标指示
Eigen::ArrayXcf LFM_MTI(Eigen::ArrayXcf Echo,
					unsigned int pulse_num,
					unsigned int pulse_length);
// MTD 动目标检测
Eigen::MatrixXcf LFM_MTD(Eigen::ArrayXcf Echo,
					unsigned int pulse_num,
					unsigned int pulse_length);

// 傅里叶变换以及逆傅里叶变换
Eigen::ArrayXcf LFM_fft(Eigen::ArrayXcf Data);
Eigen::ArrayXcf LFM_ifft(Eigen::ArrayXcf Data);

#endif