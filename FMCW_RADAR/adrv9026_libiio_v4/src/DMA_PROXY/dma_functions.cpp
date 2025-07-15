#include "DMA_PROXY/dma_functions.h"
#include <iostream>
#include <complex>
#include <cmath>


const char *tx_channel_names[] = { "dma_proxy_tx1", "dma_proxy_tx2", "dma_proxy_tx3", "dma_proxy_tx4" /* add unique channel names here */ };
const char *rx_channel_names[] = { "dma_proxy_rx1", "dma_proxy_rx2", "dma_proxy_rx3", "dma_proxy_rx4" /* add unique channel names here */ };

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
// 1. 128 FFT
// 2. 2048 FFT
// 3. Pulse Compression FFT
// 4. FMCW Pulse Compression FFT
// 5. Select Chan
// input: Selected Channel
void dma_initial(std::vector<struct channel *> & chan_tx,
              std::vector<struct channel *> & chan_rx) {
  chan_tx.clear();
  chan_rx.clear();
  for (int ii = 0; ii < 4; ii++) {
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

// FFT
void dma_FFT(std::complex<float> * data, std::complex<float> * output, 
                  struct channel * channel_tx, struct channel * channel_rx, int FFTN) {
  // transfer and receive
  dma_transfer_and_receive_data((unsigned char *)data, (unsigned char *)output, FFTN * 2 * 4, FFTN * 2 * 4, channel_tx, channel_rx);
}

// input 1: rx_buffer
// input 2: data: PN x 16384
// input 3: channel_tx
// input 4: channel_rx
// input 5: start_index offset from rx_buf
// input 6: param Pulse Radar Parameters
// input 7: the used channel 0/1/2
void dma_pulse_compression_sub(iio_buffer * rx_buf, Eigen::ArrayXXcf & data,
                  struct channel * channel_tx, struct channel * channel_rx,
                  int start_index, int start_pulse, int end_pulse, struct PulseRadar param){
  // input channels (receivers)
  const int channs = 4;
  
  if(param.siglen > _RPFFT_NUM_) {
    std::cout << "DMA Pulse Compression Warning: Param.Signal Length Too Long!\n Limit to 4096!\n";
    return;
  }
  
  if (data.cols() != _RPFFT_NUM_) {
    std::cout << "DMA Pulse Compression Error: Data Cols Must Be 4096!\n";
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
  
  for (int ii = start_pulse; ii < end_pulse; ii++) {
    memset(input_address, 0, sinput);
    memset(output_address, 0, soutput);
    // input 0 ---> arrays
    
    memcpy(input_address, start_address + start_index + ii * spri_interval, spri_interval);
    // process data
    
    dma_Select_channel((std::complex<short> *)input_address, (std::complex<float> *)output_address, channel_tx, channel_rx, sinput, channs, _RPFFT_NUM_, _RPFFT_NUM_);
    
    // remap to Eigen Matrix
    Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_address), _RPFFT_NUM_);
    data.row(ii) = array;
  }
  
  delete [] input_address;
  delete [] output_address;
}

void dma_set_channel(int chan) {
  // Select Channels for Pulse Radar Mode
  fpgaInit(_AXI_LITE_ADDRESS_);
  fpgaWrite32(xPAA_WR_DATA_REG0, chan);
  fpgaDeInit();
  fpgaInit(_AXI_LITE_SELECTBITS_);
  fpgaWrite32(xPAA_WR_DATA_REG0, chan);
  fpgaDeInit();
}

void dma_pulse_compression(iio_buffer * rx_buf, Eigen::ArrayXXcf & data,
                  std::vector<struct channel *> tx, std::vector<struct channel *> rx,
                  int start_index, struct PulseRadar param){
  // input channels (receivers)
  const int channs   = 4;
  const int axi_chan = _PULSE_RADAR_CHAN_;
  
  if(param.siglen > _RPFFT_NUM_) {
    std::cout << "DMA Pulse Compression Warning: Param.Signal Length Too Long!\n Limit to 4096!\n";
    return;
  }
  
  if (data.cols() != _RPFFT_NUM_) {
    std::cout << "DMA Pulse Compression Error: Data Cols Must Be 4096!\n";
    return;
  }
  
  std::thread _threads_[_PULSE_RADAR_CHAN_];
  for (int ii = 0; ii < axi_chan; ii++) {
    int start_pulse = ii * int(param.PN / axi_chan),
        end_pulse   = (ii+1) * int(param.PN / axi_chan);
    _threads_[ii] = std::thread(dma_pulse_compression_sub, rx_buf, std::ref(data), tx[_FFT_CHAN_OFFSET_ + ii], 
                                rx[_FFT_CHAN_OFFSET_ + ii], start_index, start_pulse, end_pulse, param);
  }
  
  for (int ii = 0; ii < axi_chan; ii++) _threads_[ii].join();
}

// can be used in before vision
// siglen should be in bytes
void dma_Select_channel(std::complex<short> * data, std::complex<float> * output,
                  struct channel * channel_tx, struct channel * channel_rx, unsigned int siglen, unsigned int channs, int packet_num, int receive_num) {
  // single transfer data number
  const unsigned int sinput  = packet_num * channs * sizeof(std::complex<short>);
  const unsigned int soutput = receive_num * sizeof(std::complex<float>);
  // loops 
  unsigned int loops = siglen / sinput + (1 * siglen % sinput == 0? 0: 1);
  unsigned char * c_iaddr = (unsigned char *)data; // current input  address
  unsigned char * c_oaddr = (unsigned char *)output; // current output address
  
  for (int ii = 0; ii < loops; ii++) {
    if (ii == 0) {
      unsigned int istep = sinput;
      unsigned int ostep = soutput;
      
      if (istep == 0) break;
      // step transfer
      dma_transfer_and_receive_data((unsigned char *)c_iaddr, (unsigned char *)c_oaddr, 
                                    istep, ostep, channel_tx, channel_rx);
    }else if (ii == loops - 1) {
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
}