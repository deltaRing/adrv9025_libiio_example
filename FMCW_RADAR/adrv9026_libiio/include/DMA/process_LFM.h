#ifndef _PROCESS_LFM_H_
#define _PROCESS_LFM_H_

#include "dma_function.h"
// eigen define
#include <Eigen/Core>
#include <Eigen/Dense>
// std
#include <complex>
#include <cstdlib>

// channel define
#define _8192_TX_Channel_ 1
#define _8192_RX_Channel_ 2
#define _1024_TX_Channel_ 3
#define _1024_RX_Channel_ 4

Eigen::MatrixXf GetDFTMatrix(int points);

// Get FFT Data
void GetFFTData(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, Eigen::ArrayXcf base_signal, axidma_dev_t axidma_dev,
                           unsigned int FFTN, unsigned int pulse, unsigned int signal);

#endif