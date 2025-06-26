#ifndef _FMCW_MIMO_ALGORITHMS_H_
#define _FMCW_MIMO_ALGORITHMS_H_

#include <cmath>
#include <vector>
#include <fftw3.h>
#include <fstream>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

// 提取脉冲数据
// 做脉冲压缩
// 输入1：发射信号
// 输入2：多通道的接收信号
// 输入3：FFT点数
// 输出1：脉冲压缩后数据
Eigen::ArrayXXcf FMCW_PulseCompression(Eigen::ArrayXcf TxSignal, Eigen::ArrayXXcf RecvSignal, int FFTN);

Eigen::ArrayXXcf Get_FMCW_FFT(Eigen::ArrayXcf TxSignal, int FFTN);

// 均值对消
// 动目标指示器
// 输入1：发射信号
// 输出1：对消后数据
Eigen::ArrayXXcf FMCW_MotionTargetIndicator(Eigen::ArrayXXcf RxSignals, int TxNum, 
                                      int PulseNum, int CancellationNum);

// 动目标检测器
Eigen::ArrayXXcf FMCW_MotionTargetDetection(Eigen::ArrayXXcf PulseCompressed,
                                      int PulseNum, int CancellationNum, int FFTN);
// 脉冲是否有效？
bool PulseValid(Eigen::ArrayXXcf _data_);

// 分割脉冲 并进行脉冲压缩
Eigen::ArrayXXcf FMCW_MultiPulseCompression(Eigen::ArrayXXcf recv, 
  Eigen::ArrayXcf TxSignal, int FFTN, int TXnum, int PulseNum);

#endif