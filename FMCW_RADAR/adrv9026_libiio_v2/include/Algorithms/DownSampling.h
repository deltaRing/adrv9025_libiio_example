#ifndef _DOWNSAMPLING_H_
#define _DOWNSAMPLING_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

Eigen::ArrayXXcf downsampling(Eigen::ArrayXXcf fb, float ofs, float efs);

#endif