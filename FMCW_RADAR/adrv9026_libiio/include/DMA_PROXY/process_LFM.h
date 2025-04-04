#ifndef _PROCESS_LFM_H_
#define _PROCESS_LFM_H_

#include "dma_functions.h"
// eigen define
#include <Eigen/Core>
#include <Eigen/Dense>
// std
#include <complex>
#include <cstdlib>
// iio
#include <iio.h>

Eigen::MatrixXcf GetDFTMatrix(int points);

// Get FFT Data
void GetFFTData(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, Eigen::ArrayXcf base_signal, 
              std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, 
              unsigned int FFTN, unsigned int pulse, unsigned int signal, int start_index);

// Get Raw Data                    
void GetRawSignal(Eigen::ArrayXXcf & indata, int enable_channels, struct iio_buffer * rx_buf,
                  struct channel * chan_tx, struct channel * chan_rx);

#endif