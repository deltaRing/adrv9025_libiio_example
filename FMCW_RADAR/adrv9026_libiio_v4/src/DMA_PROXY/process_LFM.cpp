#include "../../include/DMA_PROXY/process_LFM.h"

void MTD_SubProcess_Process(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf cinData, int start_index, int length, int cols) {  
  // Eigen Data
  Eigen::MatrixXcf p_indata = Eigen::MatrixXcf::Zero(_DFFT_NUM_, 1); // 64 x 1024
  
  Eigen::FFT<float> fft;
  std::vector<std::complex<float>> fft_out(_DFFT_NUM_);
  std::vector<std::complex<float>> fft_in(_DFFT_NUM_);
    
  for (int ii = start_index; ii < start_index + length; ii++) {
    p_indata.block(0, 0, cols, 1) = cinData.block(0, ii, cols, 1);
    for (int jj = 0; jj < _DFFT_NUM_; jj++) {
      fft_in[jj] = p_indata(jj, 0);
    }

    fft.fwd(fft_out, fft_in);

    for (int jj = 0; jj < _DFFT_NUM_; jj++) {
      outdata(jj, ii) = fft_out[jj];
    }
  }
}

void MotionTargetDetection_FMCW(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata,
              MIMO_LFM_conf conf, process_param params) {
  int cancelnum = params.cancelnum;
  unsigned int pulse = conf.PulseNum;
  unsigned int signal = conf.PulseLength;
  unsigned int tplength = conf.SignalLength;
  
  int rows = indata.rows(), cols = indata.cols();
      
  Eigen::ArrayXXcf cinData = 
        indata.block(0, 0, rows - cancelnum, cols) - 
        indata.block(cancelnum, 0, rows - cancelnum, cols);
      
  // get MTD cancellation and backgroud information
  std::thread _threads_[8];
  int length = cols / 8;
  for (int jj = 0; jj < 8; jj++) {
    if (jj < 7)
      _threads_[jj] = std::thread(MTD_SubProcess_Process, std::ref(outdata), cinData, length * jj, length, rows - cancelnum);
    else
    _threads_[jj] = std::thread(MTD_SubProcess_Process, std::ref(outdata), cinData, length * jj, cols - length * 7, rows - cancelnum);
  }
      
  for (int jj = 0; jj < 8; jj++) _threads_[jj].join();
}

void PulseCompression_FMCW(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf indata, 
              std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, 
              MIMO_LFM_conf conf, process_param params, int select_chan) {
  int cancelnum = params.cancelnum;
  unsigned int pulse = conf.PulseNum;
  unsigned int signal = conf.PulseLength;
  unsigned int tplength = conf.SignalLength;
  std::complex<float> input_buf[_RFFT_NUM_];
  std::complex<float> output_buf[_RFFT_NUM_];
  std::complex<float> * data = NULL;
  
  for (int ii = 0; ii < pulse; ii++) {
    Eigen::ArrayXXcf p_indata = indata.block(select_chan, ii * _RFFT_NUM_, 1, _RFFT_NUM_);
    data = p_indata.data();
      
    memset(input_buf, 0, sizeof(input_buf));
    memset(output_buf, 0, sizeof(output_buf));
    memcpy(input_buf, data, p_indata.cols() * sizeof(std::complex<float>));
      
    if (p_indata.cols() > _RFFT_NUM_) {
      printf("***** WARNING: Fb is greater than %d *****\n", _RFFT_NUM_);
      continue;
    }
    // dma fft
    dma_FFT(input_buf, output_buf, chan_tx[0], chan_rx[0], _RFFT_NUM_);
      
    Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), _RFFT_NUM_);
    outdata.row(ii) = array;
  }
}