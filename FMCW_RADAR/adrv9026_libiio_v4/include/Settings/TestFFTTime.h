#ifndef _TEST_FFT_TIME_H_
#define _TEST_FFT_TIME_H_

#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <fftw3.h>

using namespace std;
using namespace chrono;
using namespace Eigen;

void test_fft();

#endif