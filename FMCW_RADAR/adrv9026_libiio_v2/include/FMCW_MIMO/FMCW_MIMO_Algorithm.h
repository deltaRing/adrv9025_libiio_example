#ifndef _FMCW_MIMO_ALGORITHMS_H_
#define _FMCW_MIMO_ALGORITHMS_H_

#include <cmath>
#include <vector>
#include <fftw3.h>
#include <fstream>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

// ��ȡ��������
// ������ѹ��
// ����1�������ź�
// ����2����ͨ���Ľ����ź�
// ����3��FFT����
// ���1������ѹ��������
Eigen::ArrayXXcf FMCW_PulseCompression(Eigen::ArrayXcf TxSignal, Eigen::ArrayXXcf RecvSignal, int FFTN);

Eigen::ArrayXXcf Get_FMCW_FFT(Eigen::ArrayXcf TxSignal, int FFTN);

// ��ֵ����
// ��Ŀ��ָʾ��
// ����1�������ź�
// ���1������������
Eigen::ArrayXXcf FMCW_MotionTargetIndicator(Eigen::ArrayXXcf RxSignals, int TxNum, 
                                      int PulseNum, int CancellationNum);

// ��Ŀ������
Eigen::ArrayXXcf FMCW_MotionTargetDetection(Eigen::ArrayXXcf PulseCompressed,
                                      int PulseNum, int CancellationNum, int FFTN);
// �����Ƿ���Ч��
bool PulseValid(Eigen::ArrayXXcf _data_);

// �ָ����� ����������ѹ��
Eigen::ArrayXXcf FMCW_MultiPulseCompression(Eigen::ArrayXXcf recv, 
  Eigen::ArrayXcf TxSignal, int FFTN, int TXnum, int PulseNum);

#endif