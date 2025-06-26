#include "DMA_PROXY/dma_functions.h"
#include <iostream>
#include <complex>
#include <cmath>


const char *tx_channel_names[] = { "dma_proxy_tx1", "dma_proxy_tx2", "dma_proxy_tx3" /* add unique channel names here */ };
const char *rx_channel_names[] = { "dma_proxy_rx1", "dma_proxy_rx2", "dma_proxy_rx3" /* add unique channel names here */ };


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
  for (int ii = 0; ii < 3; ii++) {
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

// 128 FFT
void dma_FFT_128(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx) {
  // transfer and receive
  dma_transfer_and_receive_data((unsigned char *)data, (unsigned char *)output, 128 * 2 * 4, 128 * 2 * 4, channel_tx, channel_rx);
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

// input 1: rx_buffer
// input 2: data: PN x 16384
// input 3: channel_tx
// input 4: channel_rx
// input 5: start_index offset from rx_buf
// input 6: param Pulse Radar Parameters
// input 7: the used channel 0/1/2
void dma_pulse_compression(iio_buffer * rx_buf, Eigen::ArrayXXcf & data,
                  struct channel * channel_tx, struct channel * channel_rx,
                  int start_index, struct PulseRadar param, int chan){
  // input channels (receivers)
  const int channs = 4;
  
  if(param.siglen > _RPFFT_NUM_) {
    std::cout << "DMA Pulse Compression Warning: Param.Signal Length Too Long!\n Limit to 16384!\n";
    return;
  }
  
  if (data.cols() != _RPFFT_NUM_) {
    std::cout << "DMA Pulse Compression Error: Data Cols Must Be 16384!\n";
    return;
  }
  
  const unsigned int sbyte_interval = sizeof(std::complex<short>);
  const unsigned int sbyte_operate = sizeof(std::complex<float>);
  const unsigned int schan_interval = sizeof(std::complex<short>) * channs;
  const unsigned int spri_interval = sizeof(std::complex<short>) * channs * param.siglen;
  const unsigned int stransnumber = _RPFFT_NUM_; // FFT Receive Number
  const unsigned int sinput  = stransnumber * schan_interval; // input size single PRI  
  const unsigned int soutput = stransnumber * sbyte_operate; // output size single PRI
  
  char * start_address = (char *)iio_buffer_start(rx_buf);
  char * input_address = new char[sinput];
  char * output_address = new char[soutput];
  
  for (int ii = 0; ii < int(param.PN); ii++) {
    memset(input_address, 0, sinput);
    memset(output_address, 0, soutput);
    // input 0 ---> arrays
    
    memcpy(input_address, start_address + start_index + ii * spri_interval, spri_interval);
    // process data
    
    dma_Select_channel((std::complex<short> *)input_address, (std::complex<float> *)output_address, channel_tx, channel_rx, chan, sinput);
    
    // remap to Eigen Matrix
    Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_address), _RPFFT_NUM_);
    data.row(ii) = array;
  }
  
  delete [] input_address;
  delete [] output_address;
}

// can be used in before vision
// siglen should be in bytes
void dma_Select_channel(std::complex<short> * data, std::complex<float> * output,
                  struct channel * channel_tx, struct channel * channel_rx,
                  unsigned int chan, unsigned int siglen) {
  const int channs = 4;
                  
  fpgaInit(_AXI_LITE_ADDRESS_);
  fpgaWrite32(xPAA_WR_DATA_REG0, chan);
  
  // single transfer data number
  const unsigned int stransnumber = 16384 * channs;
  const unsigned int sinput  = stransnumber * sizeof(std::complex<short>);
  const unsigned int soutput = stransnumber / channs * sizeof(std::complex<float>);
  // loops 
  unsigned int loops = siglen / sinput + 1 * siglen % sinput == 0? 0: 1;
  unsigned char * c_iaddr = (unsigned char *)data; // current input  address
  unsigned char * c_oaddr = (unsigned char *)output; // current output address
  
  for (int ii = 0; ii < loops; ii++) {
    if (ii == loops - 1) {
      unsigned int istep = siglen - ii * sinput;
      unsigned int ostep = istep * sizeof(std::complex<short>) / sizeof(std::complex<float>);
      
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