//
#include "../../include/PulseRadar_LFM/PulseRadar_LFM.h" // Pulse Radar Define
#include "../../include/DMA_PROXY/process_LFM.h"         // Process Define
#include "../../include/FMCW_MIMO/FMCW_LFM.h"            // FMCW Radar Define
#include "../../include/Simulated/SimulatedSignal.h"
//
#include "dma_utilize.h"
#include "AXI_LITE_4.h"
#include <cstdlib>
#include <complex>
#include <vector>
#include <iio.h>

#define _PULSE_RADAR_CHAN_ 2
#define _FFT_CHAN_OFFSET_ 2
#define _FFT_2048_ 0
#define _FFT_4096_ 1

// input 1: the input address
// input 2: the output address
// input 3: the input data byte length
// input 4: the output data byte length
// input 5: tx channel selection
// input 6: rx channel selection
void dma_transfer_and_receive_data(unsigned char * indata, unsigned char * outdata, 
                                    unsigned int inlength, unsigned int outlength,
                                    struct channel * channel_tx, struct channel * channel_rx);

// initial dma
void dma_initial(std::vector<struct channel *> & chan_tx,
              std::vector<struct channel *> & chan_rx);
              
// deinitial dma
void dma_deinitial(std::vector<struct channel *> & chan_tx,
              std::vector<struct channel *> & chan_rx);

void dma_FFT(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx, int FFTN);
                  
void dma_Select_channel(std::complex<short> * data, std::complex<float> * output,
                  struct channel * channel_tx, struct channel * channel_rx, unsigned int siglen, unsigned int channs, int packet_num, int receive_num);

void dma_pulse_compression(iio_buffer * rx_buf, Eigen::ArrayXXcf & data,
                  std::vector<struct channel *> tx, std::vector<struct channel *> rx,
                  int start_index, struct PulseRadar param);

void dma_set_channel(int chan);
                  
void dma_FFT_IFFT_test(std::complex<float> * output, std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx);

