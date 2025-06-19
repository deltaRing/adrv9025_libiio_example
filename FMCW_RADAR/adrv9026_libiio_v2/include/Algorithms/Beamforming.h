#ifndef _BEAMFORMING_H_
#define _BEAMFORMING_H_

#define _C_ 3e8
#define _MIMO_ 0
#define _PhasedArray_ 1

#include <Eigen/Core>
#include <Eigen/Dense>

// ����Բ���е���ʸ��
// ����1��������
// ����2��������
// ����3����λ��
// ����4����Ƶ
// ����5�����а뾶
// ���1������ʸ��
Eigen::ArrayXcf CircularSteerVector(unsigned int array_num, float elevation, float azimuth, float f0, float r0);

// ���������е���ʸ��
// ����1��������
// ����2���Ƕ� // ��λ�� / ������
// ����3�����м��
// ���1������ʸ��
Eigen::ArrayXcf LinearSteerVector(unsigned int array_num, float angle, float f0, float d);

#endif