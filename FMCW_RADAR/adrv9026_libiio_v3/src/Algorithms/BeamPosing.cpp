#include "../../include/Algorithms/BeamPosing.h"

float BoseEquation(float thetaBp1, float thetaBp, float enta, unsigned int MainN) {
	return thetaBp1 * std::cos(thetaBp1) -
		(thetaBp + 0.5 * enta * 2.0 / MainN / std::cos(thetaBp)) * std::cos(thetaBp) -
		0.5 * enta * 2.0 / MainN;
}

float BoseEquationDifference(float thetaBp1, float thetaBp, float enta, unsigned int MainN) {
	return std::cos(thetaBp1) - thetaBp1 * std::sin(thetaBp1) +
		(thetaBp + 0.5 * enta * 2.0 / MainN / std::cos(thetaBp)) * std::sin(thetaBp);
}

// 利用高斯牛顿法求解非线性方程
float GaussianNewton(float thetaBp, float enta, unsigned int MainN, float sigma = 1e-12, unsigned int maxIter = 1e5) {
	float thetaBp1 = 0.0;
	for (int ii = 0; ii < maxIter; ii++) {
		float thetaBp1_ = thetaBp1 - BoseEquation(thetaBp1, thetaBp, enta, MainN) /
			BoseEquationDifference(thetaBp1, thetaBp, enta, MainN);
		float eps = abs((thetaBp1 - thetaBp1_) / thetaBp1_);
		if (eps < sigma)
			break;
		thetaBp1 = thetaBp1_;
	}
	return thetaBp1;
}

std::vector<float> BosePosing(unsigned int Channels) {
	float enta = 0.8849;
	std::vector<float> BeamDivided = {0.0};
	float thetaBp = 0.0, thetaBp1 = 0.0;
	while (thetaBp1 < 60.0) {
		// 4个阵列似乎不太行 很难形成最佳波位
		thetaBp1 = GaussianNewton(thetaBp, enta, Channels) * 180 / EIGEN_PI; // 不稳定
		if (std::abs(thetaBp1) > 60.0 && std::abs(thetaBp) < 58.0) {
			thetaBp1 = 60.0;
			BeamDivided.push_back(thetaBp1);
		}
		else if (std::abs(thetaBp1) < 60) {
			BeamDivided.push_back(thetaBp1);
		}
		thetaBp = thetaBp1 * EIGEN_PI / 180;
	}
	std::vector<float> BeamDivided_ = BeamDivided;
	for (int ii = BeamDivided.size() - 1; ii >= 1; ii--) {
		BeamDivided_.insert(BeamDivided_.begin(), -BeamDivided[ii]);
	}

	return BeamDivided;
}