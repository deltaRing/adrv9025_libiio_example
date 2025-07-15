#pragma once
#ifndef _PULSEDETECTION_H_
#define _PULSEDETECTION_H_

// Eigen ¿â
#include <Eigen/Core>
#include <Eigen/Dense>
// std ±ê×¼¿â
#include <iostream>
#include <climits>
#include <vector>
#include <thread>
#include <cmath>
// iio buffer define
#include "../../include/Utilize/common_define.h"

int PulseDetection(
	Eigen::ArrayXXcf recvEcho, 
	int PulseLength,
	int tpLength);
 
int PulseDetectionSinglePulse(Eigen::ArrayXXcf recvEcho);

int PulseDetection_LowPulse(Eigen::ArrayXXcf recvEcho, int enable_chan, int signal_length);

int PulseDetection_LowPulse(iio_buffer * rx_buf, int loop_chan, int signal_length, int enable_chans=4);

#endif