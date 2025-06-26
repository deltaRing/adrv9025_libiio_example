#ifndef _BEAMPOSING_H_
#define _BEAMPOSING_H_

// 论文来源：相控阵雷达天线最佳波位研究 王雪松
// TODO 最佳波位搜寻
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>

// 利用高斯牛顿法求解非线性方程
float GaussianNewton(float thetaBp, float enta, unsigned int MainN, float sigma, unsigned int maxIter);

// 最佳波位求解
std::vector<float> BosePosing(unsigned int Channels);

#endif