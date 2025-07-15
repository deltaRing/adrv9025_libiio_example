#ifndef _MANUAL_FFT_H_
#define _MANUAL_FFT_H_

#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
// Eigen define
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
// FFTW define
#include <fftw3.h>

// if 128 2048 fft points are un-usable
// use fftw manual compute the results
void fftw3_compute_fft(std::vector<std::complex<float>> & input, std::vector<std::complex<float>> & output, int fftn);

#endif