#include "../../include/PulseRadar_LFM/PulseRadar_LFM.h"

Eigen::MatrixXcf pDFT = GetDFTMatrix(_DPFFT_NUM_); 

// Initialize the Parameters of PulseRadar
void PulseRadarParamInit(struct PulseRadar & param) {
  param.fs  = 245.76e6;  // sample rate
  param.tp  = 2.0e-6;    // pulse width 
  param.PRI = 48.0e-6;   // PRI
  param.PN  = 64;        // pulse number
  param.B   = 20e6;      // Bandwidth
  param.c   = 3e8;       // light speed
  param._use_half_band_ = true; //
  param.K   = param.B / param.tp;
  param.siglen = (param.PRI + param.tp) * param.fs;
  param.prilen = param.PRI * param.fs;
  param.tplen  = param.tp * param.fs;
}

// Initialize the Waveform of PulseRadar
Eigen::ArrayXXcf PulseRadarWaveform(struct PulseRadar & param, std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, int enable_chan, int total_chan){
  float tp=param.tp, PRI=param.PRI, B=param.B, fs=param.fs, K=param.K;
  int siglen = param.siglen, tplen = param.tplen;
  Eigen::ArrayXXcf LFM = Eigen::ArrayXXcf::Zero(total_chan, siglen);
  
  std::cout << "Using the Pulse Radar Mode" << std::endl <<
    "Sample Rate: " << fs <<
    "Tp Length: " << tp <<
    "PRI: " << PRI <<
    "Bandwidth: " << B << std::endl;
  
  if (enable_chan < 0 || enable_chan >= total_chan) {
    assert(!"Pulse Radar Waveform: Channel enable is fault.\n");
  }  
  
  if (tp > PRI || tp < 0.0 || PRI < 0.0) {
		assert(!"Pulse Radar Waveform: Parameter tp or PRI is fault.\n");
	}
 
  Eigen::ArrayXXcf lfm(1, siglen), mf(1, _RPFFT_NUM_);
  std::complex<float> mft[_RPFFT_NUM_], mff[_RPFFT_NUM_];
  Eigen::ArrayXf t_;
  if (param._use_half_band_) t_ = Eigen::ArrayXf::LinSpaced(tplen, 0.0, tp);
  else t_ = Eigen::ArrayXf::LinSpaced(tplen, -tp / 2.0, tp / 2.0);
  // load waveform value
  for (int ii = 0; ii < _RPFFT_NUM_; ii++) {
    if (ii < tplen) {
      lfm(0, ii) = std::complex<float>{ float(cos(EIGEN_PI * K * t_(ii) * t_(ii))),
										float(sin(EIGEN_PI * K * t_(ii) * t_(ii))) };
      // conj + fliplr
      mft[_RPFFT_NUM_ - ii - 1] = std::complex<float>{ float(cos(EIGEN_PI * K * t_(ii) * t_(ii))),
										-float(sin(EIGEN_PI * K * t_(ii) * t_(ii))) };
      // need to load this value and execuate the fft operation to mf
      LFM(enable_chan, ii) = lfm(ii);
      continue;
    }
    mft[_RPFFT_NUM_ - ii - 1] = std::complex<float>{ 0, 0 };
	}
 
  struct channel * tx = chan_tx[2];
  struct channel * rx = chan_rx[2];
  dma_FFT_16384(mft, mff, tx, rx);
 
  for (int ii = 0; ii < _RPFFT_NUM_; ii++) {
    mf(0, ii) = mff[ii];
  }
  
  param.mf = mf;
  param.sig = lfm;
  return LFM;
}

// PulseCompression
void PulseCompression(Eigen::ArrayXXcf Recv, int start_index, struct PulseRadar & param, Eigen::ArrayXXcf & Return,
  std::vector<struct channel *> chan_tx, std::vector<struct channel *> chan_rx, int expect_chan) {
  Return = Eigen::ArrayXXcf::Zero(int(param.PN), _RPFFT_NUM_);
  int siglen = param.siglen,
    prilen = param.prilen;
  
  std::complex<float> input_buf[_RPFFT_NUM_];
  std::complex<float> output_buf[_RPFFT_NUM_];
  std::complex<float> output[_RPFFT_NUM_];
  std::complex<float> * data = NULL;
  struct channel * tx = chan_tx[2];
  struct channel * rx = chan_rx[2];
  struct channel * ttx = chan_tx[3];
  struct channel * rrx = chan_rx[3];
  
  for (int ii = 0; ii < int(param.PN); ii++) {
    Eigen::ArrayXXcf p_data = Recv.block(expect_chan, start_index + ii * siglen, 1, siglen);
    data = p_data.data();
    memset(input_buf, 0, sizeof(input_buf));
    memset(output_buf, 0, sizeof(output_buf));
    memcpy(input_buf, data, p_data.cols() * sizeof(std::complex<float>));
    // set the memory
    dma_FFT_16384(input_buf, output_buf, tx, rx);
    
    Eigen::Map<Eigen::ArrayXcf> mf_array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output_buf), _RPFFT_NUM_);
    Eigen::ArrayXXcf p_mf = mf_array;
    p_mf = p_mf * param.mf;
    data = p_mf.data();
    memcpy(output_buf, data, p_mf.cols() * sizeof(std::complex<float>));
    
    dma_IFFT_16384(output_buf, output, ttx, rrx);
    
    Eigen::Map<Eigen::ArrayXcf> array(reinterpret_cast<Eigen::ArrayXcf::Scalar*>(output), _RPFFT_NUM_);
    Return.row(ii) = array;
  }
}

void MTD_SubProcess(Eigen::ArrayXXcf & outdata, Eigen::ArrayXXcf cinData, int start_index, int length, int cols) {
  // Eigen Data
  Eigen::MatrixXcf p_indata = Eigen::MatrixXcf::Zero(_DPFFT_NUM_, 1); // 64 x 1024
  
  for (int ii = start_index; ii < start_index + length; ii++) {
      p_indata.block(0, 0, cols, 1) = cinData.block(0, ii, cols, 1);
      Eigen::MatrixXcf array = pDFT * p_indata;
      outdata.col(ii) = array;
  }
}

// Motion Target Detection
void MotionTargetDetection(struct PulseRadar & param, Eigen::ArrayXXcf indata, Eigen::ArrayXXcf & outdata) {
  if (pDFT.rows() != _DPFFT_NUM_) {
    // update DFT matrix
    pDFT = GetDFTMatrix(int(_DPFFT_NUM_));
  }
  
  int rows = indata.rows(), cols = indata.cols(), cancelnum = param.can_num;
    
  Eigen::ArrayXXcf cinData = 
      indata.block(0, 0, rows - cancelnum, cols) - 
      indata.block(cancelnum, 0, rows - cancelnum, cols);
  
    
  // get MTD cancellation and backgroud information
  std::thread _threads_[8];
  int length = cols / 8;
  for (int ii = 0; ii < 8; ii++) {
    if (ii < 7)
      _threads_[ii] = std::thread(MTD_SubProcess, std::ref(outdata), cinData, length * ii, length, rows - cancelnum);
    else
      _threads_[ii] = std::thread(MTD_SubProcess, std::ref(outdata), cinData, length * ii, cols - length * 7, rows - cancelnum);
  }
  
  for (int ii = 0; ii < 8; ii++) _threads_[ii].join();
}