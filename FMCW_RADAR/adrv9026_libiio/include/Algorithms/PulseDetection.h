#pragma once
#ifndef _PULSEDETECTION_H_
#define _PULSEDETECTION_H_

// Eigen ¿â
#include <Eigen/Core>
#include <Eigen/Dense>
// std ±ê×¼¿â
#include <iostream>
#include <vector>

int PulseDetection(
	Eigen::ArrayXXcf recvEcho, 
	int PulseLength,
	int tpLength);
 
int PulseDetectionSinglePulse(Eigen::ArrayXXcf recvEcho);

int PulseDetection_LowPulse(Eigen::ArrayXXcf recvEcho, int enable_chan, int signal_length);

#endif