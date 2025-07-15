#include "../../include/Algorithms/MVDR.h"

// MVDR测角
// 输入1：测得波形 channels x samples
// 输入2：方位角遍历次数
// 输入3：俯仰角遍历次数 / 如果是线阵无效
// 输入4：载频
// 输入5：阵列半径 / 如果是线阵无效
Eigen::MatrixXcf MVDR(Eigen::MatrixXcf waveform,
	unsigned int azNum, unsigned int elNum, float f0, float r0) {
	Eigen::MatrixXcf Rs  = waveform * waveform.adjoint(); // Channels x Channels
	Eigen::MatrixXcf iRs = Rs.inverse();
	Eigen::MatrixXcf angle_spectrum;
	unsigned int Channels = waveform.rows();
	float lambda = _C_ / f0;

	if (_LINE_ARRAY_) {
		angle_spectrum = Eigen::MatrixXcf::Zero(azNum, 1);
		for (int ii = 0; ii < azNum; ii++) {
			float angleAz             = 2 * EIGEN_PI / azNum * ii;
			Eigen::ArrayXcf A         = LinearSteerVector(Channels, angleAz, f0, lambda / 2); // Channels x 1
			Eigen::MatrixXcf inv_temp = A.matrix().transpose() * iRs.adjoint() * A.matrix().conjugate();
			Eigen::MatrixXcf w        = iRs * A.matrix() / inv_temp(0, 0);
			Eigen::MatrixXcf result   = w.adjoint() * Rs * w;
			angle_spectrum(ii, 0)     = result(0, 0);
		}
	}
	else if (_CIRCULAR_ARRAY_) {
		angle_spectrum = Eigen::MatrixXcf::Zero(azNum, elNum);
		for (int ii = 0; ii < azNum; ii++) {
			float angleAz = 2 * EIGEN_PI / azNum * ii;
			for (int jj = 0; jj < elNum; jj++) {
				float angleEl     = EIGEN_PI / 2.0 / azNum * ii;
				Eigen::ArrayXcf A = CircularSteerVector(Channels, angleEl, angleAz, f0, r0); // Channels x 1
				Eigen::MatrixXcf inv_temp = A.matrix().transpose() * iRs.adjoint() * A.matrix().conjugate();
				Eigen::MatrixXcf w = iRs * A.matrix() / inv_temp(0, 0);
				Eigen::MatrixXcf result = w.adjoint() * Rs * w;
				angle_spectrum(ii, jj) = result(0, 0);
			}
		}
	}
	else {
		assert("MVDR: No Valid Channel Selection");
	}
	return angle_spectrum;
}