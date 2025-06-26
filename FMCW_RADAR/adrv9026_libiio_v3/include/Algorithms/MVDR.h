#ifndef _MVDR_H_
#define _MVDR_H_

// 自定义库
#include "Beamforming.h"

#define _LINE_ARRAY_ 1
#define _CIRCULAR_ARRAY_ 0

// MVDR测角
// 输入1：测得波形
// 输入2：方位角遍历次数
// 输入3：俯仰角遍历次数 / 如果是线阵无效
// 输入4：载频
// 输入5：阵列半径 / 如果是线阵无效
Eigen::MatrixXcf MVDR(Eigen::MatrixXcf waveform, 
	unsigned int azNum, unsigned int elNum, float f0, float r0);

#endif