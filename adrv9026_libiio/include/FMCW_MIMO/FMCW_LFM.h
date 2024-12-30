#ifndef _FMCW_LFM_H_
#define _FMCW_LFM_H_

// MIMO�״������µ��״﹤��ģʽ
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fftw3.h>

#include <cmath>
#include <iostream>
//
struct MIMO_LFM_conf{
	float K;    // ��Ƶб��
	float fs;   // ������
	float B;    // ����
  float info; // ��Ϣ���� + ���
	float PRI;  // �����ظ����
	float tp;   // ������
	float PulseNum;
	unsigned int PulseLength;
	unsigned int SignalLength;
};

// ��ʼ��LFM�ź�����
void MIMO_LFM_configure(MIMO_LFM_conf & conf);

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf conf);

#endif