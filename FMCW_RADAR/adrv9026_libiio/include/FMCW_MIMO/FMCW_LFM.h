#ifndef _FMCW_LFM_H_
#define _FMCW_LFM_H_

// MIMO�״������µ��״﹤��ģʽ
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fftw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
//
#include "FMCW_MIMO_Algorithm.h"

struct MIMO_LFM_conf{
	float K;    // ��Ƶб��
	float fs;   // ������
	float B;    // ����
	float PRI;  // �����ظ����
	float tp;   // ������
	float PulseNum;
	unsigned int PulseLength;
	unsigned int SignalLength;
  Eigen::ArrayXcf FMCW_wave;
  Eigen::ArrayXcf FMCW_wave_conj;
  Eigen::ArrayXcf FFT_FMCW;
};

// ��ʼ��LFM�ź�����
void MIMO_LFM_configure(MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, bool ec[4], MIMO_LFM_conf & conf);

#endif