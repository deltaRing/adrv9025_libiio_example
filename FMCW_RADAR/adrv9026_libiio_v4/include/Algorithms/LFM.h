#ifndef _LFM_H_
#define _LFM_H_

// Eigen ���
#include <Eigen/Core>
#include <Eigen/Dense>
// FFT ���
#include <fftw3.h>
// Cmath
#include <iostream>
#include <cmath>

#define _LFM_USE_ 1
#define _PhaseCode_ 0
#define _PI_ 3.1415926535

struct LFM_conf {
	float K;    // ��Ƶб��
	float fs;   // ������
	float B;    // ����
	float PRI;  // �����ظ����
	float tp;   // ������
	unsigned int PulseNum;
	unsigned int PulseLength;
	unsigned int SignalLength;
};

// ��ʼ��LFM�ź�����
void LFM_configure(LFM_conf& conf);

// ����LFM�ź�
Eigen::ArrayXcf LFM_waveform(float fs, float B, float PRI, float tp);
// ����ƥ���˲���
Eigen::ArrayXcf LFM_matchfilter(Eigen::ArrayXcf signal);
// ����ѹ��
Eigen::ArrayXcf LFM_pulsecompression(Eigen::ArrayXcf Echo, 
									Eigen::ArrayXcf matchfilter,
									unsigned int pulse_num,
									unsigned int pulse_length);
// MTI ��Ŀ��ָʾ
Eigen::ArrayXcf LFM_MTI(Eigen::ArrayXcf Echo,
					unsigned int pulse_num,
					unsigned int pulse_length);
// MTD ��Ŀ����
Eigen::MatrixXcf LFM_MTD(Eigen::ArrayXcf Echo,
					unsigned int pulse_num,
					unsigned int pulse_length);

// ����Ҷ�任�Լ��渵��Ҷ�任
Eigen::ArrayXcf LFM_fft(Eigen::ArrayXcf Data);
Eigen::ArrayXcf LFM_ifft(Eigen::ArrayXcf Data);

#endif