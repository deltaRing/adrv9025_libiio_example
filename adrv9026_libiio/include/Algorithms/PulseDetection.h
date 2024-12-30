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
	int PulseNum,
	int PulseLength,
	int additional,
	int tpLength);

#endif