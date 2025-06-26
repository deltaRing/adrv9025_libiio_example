#ifndef _SIMULATED_SIGNAL_H_
#define _SIMULATED_SIGNAL_H_

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <random>
#include <cstdint>
#include <fstream>
// Eigen Matrix
#include <Eigen/Core>
#include <Eigen/Dense>

const double PI = 3.14159265358979323846;
const double SAMPLE_RATE = 245.76e6;    // Hz
const double PULSE_WIDTH = 10e-6;       // seconds
const double BANDWIDTH = 20e6;          // Hz
const int PRI_SAMPLES = 16384;          // samples per PRI
const int NUM_CHANNELS = 4;             // number of channels
const int16_t SCALE = 8192;            // amplitude scaling factor for short

using cshort = std::complex<short>;

using namespace Eigen;

// add noise
void add_awgn_noise_complex(std::vector<std::complex<float>>& signal, float target_snr_db);

// Generate one LFM pulse as complex<float>
std::vector<std::complex<float>> generate_lfm_pulse();

// Zero-pad to full PRI length and replicate for 4 channels
std::vector<cshort> generate_multichannel_lfm();

Eigen::ArrayXXcf generate_cos_signal();

#endif