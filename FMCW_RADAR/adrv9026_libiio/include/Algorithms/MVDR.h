#ifndef _MVDR_H_
#define _MVDR_H_

// �Զ����
#include "Beamforming.h"

#define _LINE_ARRAY_ 1
#define _CIRCULAR_ARRAY_ 0

// MVDR���
// ����1����ò���
// ����2����λ�Ǳ�������
// ����3�������Ǳ������� / �����������Ч
// ����4����Ƶ
// ����5�����а뾶 / �����������Ч
Eigen::MatrixXcf MVDR(Eigen::MatrixXcf waveform, 
	unsigned int azNum, unsigned int elNum, float f0, float r0);

#endif