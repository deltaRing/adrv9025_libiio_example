#ifndef _BEAMFORMING_H_
#define _BEAMFORMING_H_

#define _C_ 3e8
#define _MIMO_ 0
#define _PhasedArray_ 1

#include <Eigen/Core>
#include <Eigen/Dense>

// 均匀圆阵列导向矢量
// 输入1：阵列数
// 输入2：俯仰角
// 输入3：方位角
// 输入4：载频
// 输入5：阵列半径
// 输出1：导向矢量
Eigen::ArrayXcf CircularSteerVector(unsigned int array_num, float elevation, float azimuth, float f0, float r0);

// 均匀线阵列导向矢量
// 输入1：阵列数
// 输入2：角度 // 方位角 / 俯仰角
// 输入3：阵列间隔
// 输出1：导向矢量
Eigen::ArrayXcf LinearSteerVector(unsigned int array_num, float angle, float f0, float d);

#endif