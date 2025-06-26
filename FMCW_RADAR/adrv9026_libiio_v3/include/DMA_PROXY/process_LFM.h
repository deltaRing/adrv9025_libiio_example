#ifndef _PROCESS_LFM_H_
#define _PROCESS_LFM_H_

// Cancallation
#include "../Algorithms/DownSampling.h"
#include "../Utilize/common_define.h"
#include "../FMCW_MIMO/FMCW_LFM.h"
#include "dma_functions.h"
// eigen define
#include <Eigen/Core>
#include <Eigen/Dense>
// std
#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <mutex>
// fftw
#include <fftw3.h>
// iio
#include <iio.h>

using namespace std;
using namespace chrono;

struct process_param {
  float ori_fs=245.76e6;  // original sample rate
  float ex_fs=10e6;       // expected sample rate
  int cancelnum=1;        // cancellation number
  int mtd_cancel=1;       // cancellation enable
};

// Get FFT Data
void GetFFTData(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, 
              std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, 
              unsigned int FFTN, MIMO_LFM_conf conf, int txChannels,
              process_param params);

// Get Raw Data                    
void GetRawSignal(Eigen::ArrayXXcf & indata, int enable_channels, struct iio_buffer * rx_buf,
                  struct channel * chan_tx, struct channel * chan_rx);

#endif