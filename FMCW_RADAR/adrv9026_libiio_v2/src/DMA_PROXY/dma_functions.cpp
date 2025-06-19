#include "DMA_PROXY/dma_functions.h"
#include <iostream>
#include <complex>
#include <cmath>

const char *tx_channel_names[] = { "dma_proxy_tx1", "dma_proxy_tx2", "dma_proxy_tx3", "dma_proxy_tx4", "dma_proxy_tx5", /* add unique channel names here */ };
const char *rx_channel_names[] = { "dma_proxy_rx1", "dma_proxy_rx2", "dma_proxy_rx3", "dma_proxy_rx4", "dma_proxy_rx5", /* add unique channel names here */ };


// input 1: the input address
// input 2: the output address
// input 3: the input data byte length
// input 4: the output data byte length
// input 5: tx channel selection
// input 6: rx channel selection
void dma_transfer_and_receive_data(unsigned char * indata, unsigned char * outdata, 
                                    unsigned int inlength, unsigned int outlength,
                                    struct channel * channel_tx, struct channel * channel_rx) {
  unsigned int * idata = (unsigned int *)malloc(inlength),
               * rdata = (unsigned int *)malloc(outlength);
  // copy data
  memcpy(idata, indata, inlength);
  
  transfer_tx(channel_tx, idata, inlength);
  receive_rx(channel_rx, rdata, outlength);
  memcpy(outdata, rdata, outlength);
               
  free(idata);
  free(rdata);
}


// input: the use DMA channels
// 1. 8192 FFT
// 2. 2048 FFT
// 3. 16384 FFT
// 4. 16384 IFFT
// 5. Select Chan
// input: Selected Channel
void dma_initial(std::vector<struct channel *> & chan_tx,
              std::vector<struct channel *> & chan_rx) {
  chan_tx.clear();
  chan_rx.clear();
  for (int ii = 0; ii < 5; ii++) {
    struct channel * channel_tx = (struct channel *)malloc(sizeof(struct channel));
    struct channel * channel_rx = (struct channel *)malloc(sizeof(struct channel));
    
    init_channel(channel_tx, tx_channel_names[ii], true);
    init_channel(channel_rx, rx_channel_names[ii], false);
    
    chan_tx.push_back(channel_tx);
    chan_rx.push_back(channel_rx);
  }
}

void dma_deinitial(std::vector<struct channel *> & chan_tx,
              std::vector<struct channel *> & chan_rx) {
  for (int ii = 0; ii < chan_tx.size(); ii++) {
    clear_channel(chan_tx[ii]);
    clear_channel(chan_rx[ii]);
  }
  chan_tx.clear();
  chan_rx.clear();
}

// input 1: the use DMA channels
// 1. 8192 FFT
// 2. 2048 FFT
// 3. 16384 FFT
// 4. 16384 IFFT
// 5. Select Chan
// input 2: Selected Channel
void dma_FFT_8192(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx) {
  // transfer and receive
  dma_transfer_and_receive_data((unsigned char *)data, (unsigned char *)output, 8192 * 2 * 4, 8192 * 2 * 4, channel_tx, channel_rx);
  
}
                  
void dma_FFT_2048(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx) {
                  
  // transfer and receive
  dma_transfer_and_receive_data((unsigned char *)data, (unsigned char *)output, 2048 * 2 * 4, 2048 * 2 * 4, channel_tx, channel_rx);
}

void dma_FFT_16384(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx) {
                  
  // transfer and receive
  dma_transfer_and_receive_data((unsigned char *)data, (unsigned char *)output, 16384 * 2 * 4, 16384 * 2 * 4, channel_tx, channel_rx);
}

void dma_IFFT_16384(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx) {
                  
  // transfer and receive
  dma_transfer_and_receive_data((unsigned char *)data, (unsigned char *)output, 16384 * 2 * 4, 16384 * 2 * 4, channel_tx, channel_rx);
}

void dma_FFT_IFFT_test(std::complex<float> * output, std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx) {
  std::complex<float> input[16384], outbuf[16384];
  float fs = 1e5, f0 = 1e3, t = 1.0 / 1e5;
  for (int ii = 0; ii < 16384; ii++) {
    input[ii] = std::complex<float> {cos(t * 2 * 3.1415926535 * f0 * ii),
                                    sin(t * 2 * 3.1415926535 * f0 * ii)};
  }
  struct channel * tx = chan_tx[2];
  struct channel * rx = chan_rx[2];
  struct channel * ttx = chan_tx[3];
  struct channel * rrx = chan_rx[3];
  
  dma_FFT_16384(input, outbuf, tx, rx);
  dma_IFFT_16384(outbuf, output, ttx, rrx);
}

// siglen should be in bytes
void dma_Select_channel(std::complex<short> * data, std::complex<float> * output,
                  struct channel * channel_tx, struct channel * channel_rx,
                  unsigned int chan, unsigned int siglen) {
  const int channs = 4;
                  
  fpgaInit(_AXI_LITE_ADDRESS_);
  fpgaWrite32(xPAA_WR_DATA_REG0, chan);
  
  // single transfer data number
  const unsigned int stransnumber = 16384 * 2;
  const unsigned int sinput  = stransnumber * sizeof(std::complex<short>);
  const unsigned int soutput = stransnumber / channs * sizeof(std::complex<float>);
  // loops 
  unsigned int loops = siglen / sinput + 1;
  unsigned char * c_iaddr = (unsigned char *)data; // current input  address
  unsigned char * c_oaddr = (unsigned char *)output; // current output address
  
  // std::cout << sinput << " " << soutput << " " << loops << " " << siglen << std::endl;
  
  for (int ii = 0; ii < loops; ii++) {
    if (ii == loops - 1) {
      unsigned int istep = siglen - ii * sinput;
      unsigned int ostep = istep * sizeof(std::complex<short>) / sizeof(std::complex<float>);
      
      // std::cout << istep << " " << ostep << std::endl;
      
      if (istep == 0) break;
      // step transfer
      dma_transfer_and_receive_data((unsigned char *)c_iaddr, (unsigned char *)c_oaddr, 
                                    istep, ostep, channel_tx, channel_rx);
      break; // doing complete
    }else {
      // step transfer
      dma_transfer_and_receive_data((unsigned char *)c_iaddr, (unsigned char *)c_oaddr, 
                                    sinput, soutput, channel_tx, channel_rx);
      c_iaddr += sinput;
      c_oaddr += soutput;
    }
  }
  
  fpgaDeInit();
}