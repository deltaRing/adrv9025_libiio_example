#include "../../include/Algorithms/Beamforming.h"

// 均匀圆阵列导向矢量
// 输入1：阵列数 (转换为 rad 单位)
// 输入2：俯仰角 (转换为 rad 单位)
// 输入3：方位角
// 输入4：载频
// 输入5：阵列半径
// 输出1：导向矢量
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

// 均匀线阵列导向矢量
// 输入1：阵列数
// 输入2：角度 // 方位角 / 俯仰角
// 输入3：阵列间隔
// 输出1：导向矢量
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