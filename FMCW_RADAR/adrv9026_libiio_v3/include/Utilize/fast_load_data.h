#ifndef _FAST_LOAD_DATA_H_
#define _FAST_LOAD_DATA_H_

//
#include "../../include/Algorithms/PulseDetection.h"
// Matrix
#include <Eigen/Core>
#include <Eigen/Dense>
// Std Include Define
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <complex>
#include <thread>
#include <vector>
#include <cmath>
// IIO Define
#include <iio.h>

// write the data
void write_iio_data_bin(std::string name, struct iio_buffer * rx_buf);

// fast load data
void load_iio_data(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, struct iio_buffer * rx_buf,
                       int pulse_num, int tpLen, int sigLen, int enable_chan, int loop_chan);
                       
void load_iio_data_multi(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, struct iio_buffer * rx_buf, 
                        int pulse_num, int tpLen, int sigLen, int enable_chan, int loop_chan);

#endif