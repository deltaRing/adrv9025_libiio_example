#include "../../include/DMA_PROXY/process_LFM.h"

Eigen::MatrixXcf DFT = GetDFTMatrix(_DFFT_NUM_);
std::mutex _fftw_locker_;

void MTD_SubProcess_Process(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf cinData, int start_index, int length, int cols) {  
  // Eigen Data
  Eigen::MatrixXcf p_indata = Eigen::MatrixXcf::Zero(_DFFT_NUM_, 1); // 64 x 1024
  
  for (int ii = start_index; ii < start_index + length; ii++) {
      p_indata.block(0, 0, cols, 1) = cinData.block(0, ii, cols, 1);
      Eigen::MatrixXcf array = DFT * p_indata;
      outdata.col(ii) = array;
  }
}

void MTD_SubProcess_Process_FFTW(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf cinData, int start_index, int length, int cols) {  
  // Eigen Data
  Eigen::MatrixXcf p_indata = Eigen::MatrixXcf::Zero(_DFFT_NUM_, 1); // 64 x 1024
  std::vector<std::complex<float>> X_fftw_st(_DFFT_NUM_);
  
  for (int ii = start_index; ii < start_index + length; ii++) {
      p_indata.block(0, 0, cols, 1) = cinData.block(0, ii, cols, 1);
      
      _fftw_locker_.lock();
      fftwf_plan plan_st = fftwf_plan_dft_1d(_DFFT_NUM_,
        reinterpret_cast<fftwf_complex*>(p_indata.data()),
        reinterpret_cast<fftwf_complex*>(X_fftw_st.data()),
        FFTW_FORWARD, FFTW_ESTIMATE);
      fftwf_execute(plan_st);
      
      fftwf_destroy_plan(plan_st);
      _fftw_locker_.unlock();
      
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(X_fftw_st.data()), _DFFT_NUM_);
      outdata.col(ii) = array;
  }
}

void GetFFTData(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, 
              std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, 
              unsigned int FFTN,  MIMO_LFM_conf conf, int txChannels, 
              process_param params) {
              
  float ori_fs  = params.ori_fs;
  float ex_fs   = params.ex_fs;
  int cancelnum = params.cancelnum;
  unsigned int pulse = conf.PulseNum;
  unsigned int signal = conf.PulseLength;
  unsigned int tplength = conf.SignalLength;
  
  if (FFTN == 8192) {
    std::complex<float> input_buf[8192];
    std::complex<float> output_buf[8192];
    std::complex<float> * data = NULL;
    struct channel * tx = chan_tx[0];
    struct channel * rx = chan_rx[0];
    for (int ii = 0; ii < pulse; ii++) {
      Eigen::ArrayXXcf p_indata = indata.block(0, ii * tplength, 1, tplength);
      data = p_indata.data();
      // data = p_indata.data();
      memset(input_buf, 0, sizeof(input_buf));
      memset(output_buf, 0, sizeof(output_buf));
      // TX1 to RX1 length = 4915
      memcpy(input_buf, data, p_indata.cols() * sizeof(std::complex<float>));
      
      if (p_indata.cols() >= 8192) {
        printf("***** WARNING: Fb is greater than 8192 *****\n");
        continue;
      }
      
      // dma fft
      dma_FFT_8192(input_buf, output_buf, tx, rx);
      
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), 8192);
      outdata.row(ii) = array;
    }
  }
  // For MTD
  else if (FFTN == _RFFT_NUM_) {
    std::complex<float> input_buf[_RFFT_NUM_];
    std::complex<float> output_buf[_RFFT_NUM_];
    std::complex<float> * data = NULL;
    struct channel * tx = chan_tx[1];
    struct channel * rx = chan_rx[1];
    for (int ii = 0; ii < pulse; ii++) {
      Eigen::ArrayXXcf p_indata = indata.block(0, ii * tplength, 1, tplength);
      Eigen::ArrayXXcf dfb = downsampling(p_indata, ori_fs, ex_fs); 
       
      data = dfb.data();
      // data = p_indata.data();
      memset(input_buf, 0, sizeof(input_buf));
      memset(output_buf, 0, sizeof(output_buf));
      // TX1 to RX1 length = 4915
      memcpy(input_buf, data, dfb.cols() * sizeof(std::complex<float>));
      
      if (dfb.cols() >= _RFFT_NUM_) {
        printf("***** WARNING: Fb is greater than 2048 *****\n");
        continue;
      }
      // dma fft
      dma_FFT_2048(input_buf, output_buf, tx, rx);
      
      Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), _RFFT_NUM_);
      outdata.row(ii) = array;
    }
  } else if (FFTN == _DFFT_NUM_) {
    int rows = indata.rows(), cols = indata.cols();
    
    Eigen::ArrayXXcf cinData = 
      indata.block(0, 0, rows - cancelnum, cols) - 
      indata.block(cancelnum, 0, rows - cancelnum, cols);
    
    // get MTD cancellation and backgroud information
    std::thread _threads_[8];
    int length = cols / 8;
    for (int ii = 0; ii < 8; ii++) {
      if (ii < 7)
        _threads_[ii] = std::thread(MTD_SubProcess_Process, std::ref(outdata), cinData, length * ii, length, rows - cancelnum);
      else
        _threads_[ii] = std::thread(MTD_SubProcess_Process, std::ref(outdata), cinData, length * ii, cols - length * 7, rows - cancelnum);
    }
    
    for (int ii = 0; ii < 8; ii++) _threads_[ii].join();
    
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