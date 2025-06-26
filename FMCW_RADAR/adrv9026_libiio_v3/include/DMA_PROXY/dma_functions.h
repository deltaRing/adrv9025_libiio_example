//
#include "../../include/PulseRadar_LFM/PulseRadar_LFM.h"
#include "../../include/Simulated/SimulatedSignal.h"
//
#include "dma_utilize.h"
#include "AXI_LITE_4.h"
#include <cstdlib>
#include <complex>
#include <vector>
#include <iio.h>

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

// 128 FFT
void dma_FFT_128(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx);

// input 1: the use DMA channels
// 1. 8192 FFT
// 2. 2048 FFT
// 3. 16384 FFT
// 4. 16384 IFFT
// 5. Select Chan
// input 2: Selected Channel
void dma_FFT_8192(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx);
                  
void dma_FFT_2048(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx);
                  
void dma_FFT_16384(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx);

void dma_IFFT_16384(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx);
                  
void dma_Select_channel(std::complex<short> * data, std::complex<float> * output,
                  struct channel * channel_tx, struct channel * channel_rx,
                  unsigned int chan, unsigned int siglen);

void dma_pulse_compression(iio_buffer * rx_buf, Eigen::ArrayXXcf & data,
                  struct channel * channel_tx, struct channel * channel_rx,
                  int start_index, struct PulseRadar param, int chan);
                  
void dma_FFT_IFFT_test(std::complex<float> * output, std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx);

