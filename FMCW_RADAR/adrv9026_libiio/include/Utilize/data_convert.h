#ifndef _DATA_CONVERT_H_
#define _DATA_CONVERT_H_

#include <algorithm>
#include <iterator>
#include <iostream>
#include <complex>
#include <thread>
#include <cmath>
//
#include <Eigen/Core>
#include <Eigen/Dense>
// iio definition
#include <iio.h>
// arm ���ָ�
#include <arm_neon.h>
// fftw
#include <fftw3.h>
#include <unistd.h>

// Eigen cost: 0.07*--0.05
void GetData(Eigen::ArrayXXcf & data, struct iio_buffer * rx_buf);
void GetData_MultiThread(Eigen::ArrayXXcf & data, struct iio_buffer * rx_buf, int ThreadNumber);
void GetDataSubProcess(Eigen::ArrayXXcf & stored_data, int start_index, 
      int enable_channel_num, char * start_address, char * end_address, ptrdiff_t p_inc);
      
// for short 16 cost 0.22--0.20
void GetData16(std::complex<int16_t> * data, int enable_channel_num, int sample_num, struct iio_buffer * rx_buf);
void GetData16SubProcess(std::complex<int16_t> * stored_data, int start_index, int sample_num, 
      int enable_channel_num, char * start_address, char * end_address, ptrdiff_t p_inc);
void GetData16_MultiThread(std::complex<int16_t> * data, int enable_channel_num,
     int sample_num, struct iio_buffer * rx_buf, int ThreadNumber);

#endif