#ifndef _LOL_H_
#define _LOL_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <fftw3.h>

#include <cmath>
#include <fstream>
#include <iostream>

Eigen::ArrayXXcf LOL_Generate(int channel_num, int length=245760);

#endif