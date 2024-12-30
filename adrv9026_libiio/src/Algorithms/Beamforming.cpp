#include "../../include/Algorithms/Beamforming.h"

// ����Բ���е���ʸ��
// ����1�������� (ת��Ϊ rad ��λ)
// ����2�������� (ת��Ϊ rad ��λ)
// ����3����λ��
// ����4����Ƶ
// ����5�����а뾶
// ���1������ʸ��
Eigen::ArrayXcf CircularSteerVector(unsigned int array_num, float elevation, float azimuth, float f0, float r0) {
	Eigen::ArrayXcf SteerVector(array_num);
	float lambda = _C_ / f0;
	for (int ii = 0; ii < array_num; ii++) {
		float tau = r0 / lambda * std::cos(azimuth - 2 * EIGEN_PI * ii / array_num) * std::sin(elevation);
		SteerVector(ii) = std::complex<float>{
			float(std::cos(2 * EIGEN_PI * tau)), float(std::sin(2 * EIGEN_PI * tau))
		};
	}
	return SteerVector;
}

// ���������е���ʸ��
// ����1��������
// ����2���Ƕ� // ��λ�� / ������
// ����3�����м��
// ���1������ʸ��
Eigen::ArrayXcf LinearSteerVector(unsigned int array_num, float angle, float f0, float d) {
	Eigen::ArrayXcf SteerVector(array_num);
	float lambda = _C_ / f0;
	for (int ii = 0; ii < array_num; ii++) {
		float tau = 2 * EIGEN_PI / lambda * d * std::sin(angle) * ii; 
		SteerVector(ii) = std::complex<float>{
			float(std::cos(tau)), float(std::sin(tau))
		};
	}
	return SteerVector;
}