#ifndef _RADAR_FUNCTIONS_H_
#define _RADAR_FUNCTIONS_H_

// Phased Array Radar Functions
#include "include/Algorithms/LFM.h"
#include "include/Algorithms/Beamforming.h"
#include "include/Algorithms/PulseDetection.h"
// Load Waveforms and Set up Adrv9025
#include "include/Utilize/read_configuration.h"
#include "include/Utilize/fast_load_data.h"
#include "include/Utilize/data_convert.h"
#include "include/Utilize/iio_adrv9025.h"
#include "include/Utilize/iio_adrv9009.h"
// FMCW-MIMO Radar Algorithms
#include "include/FMCW_MIMO/FMCW_LFM.h"
#include "include/FMCW_MIMO/1D_CACFAR.h"
#include "include/FMCW_MIMO/FMCW_MIMO_Algorithm.h"
// Standard Functions
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
// Commonly Used define
#include "include/Utilize/common_define.h"
#include "include/Settings/UsingDifferentModule.h"
// DMA Define
#include "include/DMA_PROXY/dma_functions.h"
#include "include/DMA_PROXY/process_LFM.h"
// UDP 
#include "include/Networks/UDP_Networks.h"
// TCP
#include "include/Networks/TCP_Networks.h"

// Transfer and Receive 
void Send_Recv(int16_t * _data_tx_, int16_t * _data_rx_, bool & new_data);

void Send_Recv_PulseRadar (int16_t * _data_tx_, int16_t * _data_rx_, bool & new_data);

void Send_Recv_LoL(int16_t * _data_tx_, int16_t * _data_rx_);

#endif