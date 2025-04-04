#include "../../include/DMA_PROXY/process_LFM.h"
#include <iostream>

Eigen::MatrixXcf GetDFTMatrix(int points) {
  Eigen::MatrixXcf DFT(points, points);
  
  for (int ii = 0; ii < points; ii++) {
    for (int jj = 0; jj < points; jj++) {
      DFT(ii, jj) = std::complex<float> {
        cos(-2 * 3.1415926535 * ii * jj / points) / sqrt(points),
        sin(-2 * 3.1415926535 * ii * jj / points) / sqrt(points)
      };
    }
  }

  return DFT;
}

void GetFFTData(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, Eigen::ArrayXcf base_signal, 
              std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, 
              unsigned int FFTN, unsigned int pulse, unsigned int signal, int start_index) {
  static Eigen::MatrixXcf DFT = GetDFTMatrix(64);
  int txChannels              = indata.rows();
  if (FFTN == 8192) {
    std::complex<float> input_buf[8192];
    std::complex<float> output_buf[8192];
    std::complex<float> * data = NULL;
    struct channel * tx = chan_tx[0];
    struct channel * rx = chan_rx[0];
    for (int ii = 0; ii < pulse; ii++) {
      Eigen::ArrayXcf p_indata = indata.row(0).block(start_index + ii * signal, 0, signal / txChannels, 1);        
      p_indata = p_indata * base_signal;
      p_indata = p_indata - p_indata.abs().mean();
      data = p_indata.data();    
      memset(input_buf, 0, sizeof(input_buf));
      memset(output_buf, 0, sizeof(output_buf));
      // TX1 to RX1 length = 4915
      memcpy(input_buf, data, signal / txChannels * sizeof(std::complex<float>));
      
      // dma fft
      dma_FFT_8192(input_buf, output_buf, tx, rx);
      
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), 8192);
      outdata.row(ii) = array;
    }
  }
  // For MTD
  else if (FFTN == 1024) {
    std::complex<float> input_buf[1024];
    std::complex<float> output_buf[1024];
    std::complex<float> * data = NULL;
    struct channel * tx = chan_tx[1];
    struct channel * rx = chan_rx[1];
    for (int ii = 0; ii < 8192; ii++) {
      Eigen::ArrayXcf p_indata = indata.row(0).block(0, ii, pulse, 1);   
      data = p_indata.data();  
      memset(input_buf, 0, sizeof(input_buf));
      memset(output_buf, 0, sizeof(output_buf));
      // TX1 to RX1 length = 4915
      memcpy(input_buf, data, pulse * sizeof(std::complex<float>));
      
      // dma fft
      dma_FFT_1024(input_buf, output_buf, tx, rx);
      
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), 1024);
      outdata.col(ii) = array;
    }
  } else if (FFTN == 64) {
    for (int ii = 0; ii < 8192; ii++) {
      Eigen::MatrixXcf p_indata = Eigen::MatrixXcf::Zero(64, 1);
      p_indata = indata.block(0, ii, pulse, 1);
      Eigen::MatrixXcf array = DFT * p_indata;
      outdata.col(ii) = array.array();
    }
  } else {
    // Error 
    
    
  }
}

// Get Raw Data
void GetRawSignal(Eigen::ArrayXXcf & indata, int enable_channels, struct iio_buffer * rx_buf,
                struct channel * chan_tx, struct channel * chan_rx) {
  // get data and size
  unsigned int byte_size = abs((unsigned char *)iio_buffer_start(rx_buf) - (unsigned char *)iio_buffer_end(rx_buf));
  unsigned int out_size  = byte_size / sizeof(std::complex<short>) / enable_channels * 
                          sizeof(std::complex<float>); 
  unsigned int cols = out_size / sizeof(std::complex<float>);
  std::complex<short> * data = (std::complex<short> *)malloc(byte_size);
  std::complex<float> * out  = (std::complex<float> *)malloc(out_size); // fixed for adrv9025
  memcpy(data, iio_buffer_start(rx_buf), byte_size);
  
  for (int ii = 0; ii < indata.rows(); ii++) {
    dma_Select_channel(data, out, chan_tx, chan_rx, ii, byte_size);
    Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(out), cols);
    indata.row(ii) = array;
  }
  
  free(data);
  free(out);
}