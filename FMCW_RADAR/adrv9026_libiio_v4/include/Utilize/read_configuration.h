#ifndef _READ_CONFIGURATION_H_
#define _READ_CONFIGUARTION_H_

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
// eigen define
#include <Eigen/Core>
#include <Eigen/Dense>
// iio and signal define 
#include "../../include/FMCW_MIMO/FMCW_LFM.h"
#include "../../include/FMCW_MIMO/1D_CACFAR.h"
#include "../../include/DMA_PROXY/process_LFM.h"
#include "../../include/Utilize/common_define.h"
#include "../../include/PulseRadar_LFM/PulseRadar_LFM.h"

// example define
struct parsed_config{
// Basical Configuration
  float fs = 245.76e6; // fixed number DO NOT MODIFY
// MIMO Radar Configuration
  float B;             // used bandwidth
  float PRI;           //
  float tp;            //
  float PulseNum;      // Pulse Number
// Pulse Radar Configuration
  float Bd;
// MIMO Signal Processing Parameter
  float RFFT;          // Range FFT
  float DFFT;          // Doppler FFT
// Radar Type 
  float type;          // 0: MIMO Radar, 1: Pulse Radar, 2: Hybird Radar
// Enable TX and RX
  float ec[4];        // 1~4: if enable 1, else 0
// LO
  float LO;            // carry frequency
};

bool read_config_file(std::string filename, 
  std::unordered_map<std::string, std::vector<float>> & confData);
  
void parse_config_file(std::unordered_map<std::string, std::vector<float>> & confData, 
  int & fmcw_enable,
  struct MIMO_LFM_conf & config,
  struct x1d_cacfar_param & xparams,
  struct PulseRadar & pconfig,
  struct stream_cfg & trxcfg,
  struct process_param & params,
  bool ec[4]);


#endif