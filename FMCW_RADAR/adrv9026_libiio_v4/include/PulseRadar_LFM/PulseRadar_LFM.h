#ifndef _PULSERADAR_LFM_H_
#define _PULSERADAR_LFM_H_

#include <cmath>
#include <thread>
#include <vector>
#include <climits>
#include <fstream>
#include <iostream>
// Eigen Defination
#include <Eigen/Core>
#include <Eigen/Dense>
// DMA FFT Defination
#include "../../include/DMA_PROXY/dma_functions.h"
#include "../../include/DMA_PROXY/process_LFM.h"
// Build MatrixXcf
#include "../../include/Utilize/common_define.h"
#include "../../include/Utilize/manual_fft.h"

struct PulseRadar{
  float fs      = 245.76e6; // fixed sample rate
  float tp      = 2.0e-6;   // pulse width 
  float PRI     = 48.0e-6;  // PRI
  float PN      = 32;       // pulse number
  float B       = 20e6;     // Bandwidth
  float c       = 3e8;      // light speed
  float can_num = 1;     // cancenllation number
  float K;               // slope 
  bool  _use_half_band_; //
  int siglen;            // signal length
  int prilen;            // PRI length
  int tplen;             // tp length
  Eigen::ArrayXXcf mf;   // match filter
  Eigen::ArrayXXcf sig;  // emit signal
};

// Initialize the Parameters of PulseRadar
void PulseRadarParamInit(struct PulseRadar & param);

// Initialize the Waveform of PulseRadar
Eigen::ArrayXXcf PulseRadarWaveform(struct PulseRadar & param, std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, int enable_chan=0, int total_chan=4);

// Motion Target Detection
void MotionTargetDetection(struct PulseRadar & param, Eigen::ArrayXXcf indata, Eigen::ArrayXXcf & outdata);

#endif