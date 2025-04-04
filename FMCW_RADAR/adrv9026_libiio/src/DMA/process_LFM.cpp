#include "../../include/DMA/process_LFM.h"

Eigen::MatrixXf GetDFTMatrix(int points) {
  Eigen::MatrixXf DFT(points, points);
  
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

// Get FFT Data
void GetFFTData(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, Eigen::ArrayXcf base_signal, axidma_dev_t axidma_dev,
                           Eigen::MatrixXf DFT, unsigned int FFTN, unsigned int pulse, unsigned int signal) {
  if (FFTN == 8192) {
    std::complex<float> input_buf[8192];
    std::complex<float> output_buf[8192];
    std::complex<float> * data = NULL;
    for (int ii = 0; ii < pulse; ii++) {
      Eigen::ArrayXcf p_indata = indata.block(ii * signal, 0, signal / 4, 1);        
      p_indata = p_indata * base_signal;
      data = p_indata.data();    
      memset(input_buf, 0, sizeof(input_buf));
      memset(output_buf, 0, sizeof(output_buf));
      // TX1 to RX1 length = 4915
      memcpy(input_buf, data, signal / 4 * sizeof(std::complex<float>));
      int ret = DMA_FFT(axidma_dev, _8192_TX_Channel_, _8192_RX_Channel_, 8192, 8192, input_buf, output_buf);
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), 8192);
      outdata.row(ii) = array;
    }
    // std::cout << "Successfully Processed PulseCompressed" << std::endl;
  }
  else if (FFTN == 1024) {
    std::complex<float> input_buf[1024];
    std::complex<float> output_buf[1024];
    std::complex<float> * data = NULL;
    for (int ii = 0; ii < 8192; ii++) {
      Eigen::ArrayXcf p_indata = indata.block(0, ii, pulse, 1);   
      data = p_indata.data();  
      memset(input_buf, 0, sizeof(input_buf));
      memset(output_buf, 0, sizeof(output_buf));
      // TX1 to RX1 length = 4915
      memcpy(input_buf, data, pulse * sizeof(std::complex<float>));
      int ret = DMA_FFT(axidma_dev, _1024_TX_Channel_, _1024_RX_Channel_, 1024, 1024, input_buf, output_buf);
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), 1024);
      outdata.col(ii) = array;
    }
  } else if (FFTN == 64) {
    for (int ii = 0; ii < 8192; ii++) {
      Eigen::MatrixXcf p_indata = Eigen::MatrixXcf::Zero(64, 1);
      p_indata = indata.block(0, ii, pulse, 1)
      Eigen::MatrixXcf array = DFT * p_indata;
      outdata.col(ii) = array.array();
    }
  } else {
   // Error 
  
  
  }
}