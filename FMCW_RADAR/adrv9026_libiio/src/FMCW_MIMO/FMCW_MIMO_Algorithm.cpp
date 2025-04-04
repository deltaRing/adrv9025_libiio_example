#include "include/FMCW_MIMO/FMCW_MIMO_Algorithm.h"

// 提取脉冲数据
// 做脉冲压缩
// 输入1：发射信号
// 输入2：多通道的接收信号
// 输入3：FFT点数
// 输出1：脉冲压缩后数据
Eigen::ArrayXXcf FMCW_PulseCompression(Eigen::ArrayXcf TxSignal, Eigen::ArrayXXcf RecvSignal, int FFTN){
  int length = TxSignal.rows(),
    channels = RecvSignal.rows();
  
  Eigen::ArrayXXcf _ProcSignals_(channels, length); 
  Eigen::ArrayXXcf _PulseCompressed_ = Eigen::ArrayXXcf::Zero(channels, FFTN);
  fftw_complex * din = NULL, * out = NULL;
	fftw_plan p;
	din = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * length);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFTN);
 
  for (int ii = 0; ii < channels; ii++){
    Eigen::ArrayXcf _ConjTxSignal_  = TxSignal.conjugate();
    Eigen::ArrayXcf _RowRecvSignal_ = RecvSignal.row(ii);
    Eigen::ArrayXcf _ProcSignals_   = _RowRecvSignal_ * _ConjTxSignal_;
    for (int iii = 0; iii < length; iii++) {
      din[iii][0] = _ProcSignals_(iii).real();
  		din[iii][1] = _ProcSignals_(iii).imag();
    }
    p = fftw_plan_dft_1d(FFTN, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
  	fftw_execute(p); /* repeat as needed */
  	fftw_destroy_plan(p);
  	fftw_cleanup();
    for (int iii = 0; iii < FFTN; iii++) {
      _PulseCompressed_(ii, iii) = std::complex<float> { out[iii][0], out[iii][1] };
    }
  }
 
	fftw_free(din);
	fftw_free(out);
  
  return _PulseCompressed_;
}


Eigen::ArrayXcf Get_FMCW_FFT(Eigen::ArrayXcf TxSignal, int FFTN) {
  Eigen::ArrayXcf FFT(FFTN);
  fftw_complex * din = NULL, * out = NULL;
	fftw_plan p;
	din = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFTN);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFTN);
 
  for (int iii = 0; iii < FFTN; iii++) {
    if (iii < TxSignal.size()) {
      din[iii][0] = TxSignal(iii).real();
		  din[iii][1] = TxSignal(iii).imag();
    }else {
      din[iii][0] = 0;
      din[iii][1] = 0;
    }
  }
  
  p = fftw_plan_dft_1d(FFTN, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
 	fftw_execute(p); /* repeat as needed */
 	fftw_destroy_plan(p);
 	fftw_cleanup();
  
  for (int iii = 0; iii < FFTN; iii++) {
    FFT(iii) = std::complex<float> { out[iii][0], out[iii][1] };
  }
 
  fftw_free(din);
	fftw_free(out);
  
  return FFT;

}

// 均值对消
// 动目标指示器
// 输入1：接收信号
// 输出1：对消后数据
Eigen::ArrayXXcf FMCW_MotionTargetIndicator(Eigen::ArrayXXcf RxSignals, int TxNum, 
                                      int PulseNum, int CancellationNum) {

  if (PulseNum <= CancellationNum) {
    assert(!"MTI: Cancellation Number should be less than Pulse Number");
  }else if (CancellationNum <= 0) {
    assert(!"MTI: Cancellation Number should be greater than zero");
  }

  int RxNum = RxSignals.rows(), totalsamples = RxSignals.cols();
  int part_samples = totalsamples / TxNum / PulseNum;
  
  Eigen::ArrayXXcf _PartMTI_ = RxSignals.block(0, 0, RxNum, part_samples * PulseNum);
  Eigen::ArrayXXcf _MTI_ = _PartMTI_.block(0, 0, RxNum, (PulseNum - CancellationNum) * part_samples) -
    _PartMTI_.block(0, CancellationNum * part_samples - 1, RxNum, (PulseNum - CancellationNum) * part_samples);
  
  return _MTI_;
}

// 动目标检测器
Eigen::ArrayXXcf FMCW_MotionTargetDetection(Eigen::ArrayXXcf PulseCompressed, int PulseNum, 
                                      int CancellationNum, int FFTN) {
  int iterNum = PulseCompressed.cols() / (PulseNum - CancellationNum);
  fftw_complex * din = NULL, * out = NULL;
	fftw_plan p;
	din = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFTN);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFTN);
  Eigen::ArrayXXcf _MTD_result_(iterNum, FFTN);
  
  for (int ii = 0; ii < iterNum; ii++) {
    for (int iii = 0; iii < FFTN; iii++) {
      if (iii < PulseNum - CancellationNum) {
        std::complex <float> temp = PulseCompressed(0, ii + iii * iterNum);
        din[iii][0] = temp.real();
   		  din[iii][1] = temp.imag();
      }else{
        din[iii][0] = 0.0;
        din[iii][1] = 0.0;
      }
    }
    p = fftw_plan_dft_1d(FFTN, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
   	fftw_execute(p); /* repeat as needed */
   	fftw_destroy_plan(p);
   	fftw_cleanup();
    for (int iii = 0; iii < FFTN; iii++) {
      _MTD_result_(ii, (iii + FFTN / 2) % FFTN) = std::complex<float> {
        out[iii][0], out[iii][1]
      };
    }
  }
  
	fftw_free(din);
	fftw_free(out);
  
  return _MTD_result_;
}

bool PulseValid(Eigen::ArrayXXcf _data_){
  float max_ = _data_.row(0).abs().maxCoeff(),
  min_ = _data_.row(0).abs().minCoeff();
  
  if (max_ < 75) { 
    return false; 
  }
  else{
    if (max_ > min_ * 1.5) 
      return true;
    else
      return false;
  }
}

Eigen::ArrayXXcf FMCW_MultiPulseCompression(Eigen::ArrayXXcf recv, 
  Eigen::ArrayXcf TxSignal, int FFTN, int TXnum, int PulseNum) {
  
  int RXnum = recv.rows();                     // 接收天线数目
  int totalsamples = recv.cols();              // 总采样数
  int singlesamples = totalsamples / PulseNum; // 单采样数 （should be Tx x length）
  int length = TxSignal.rows();                // 
  
  std::cout << length << " " << singlesamples << std::endl;
  
  Eigen::ArrayXXcf res(RXnum, TXnum * PulseNum * FFTN);
  for (int pp = 0; pp < PulseNum; pp++) {
    Eigen::ArrayXXcf single_pulse = recv.block(0, pp * singlesamples, RXnum, singlesamples);
    
    // 单脉冲数据
    for (int tt = 0; tt < TXnum; tt++) {
      // 
      Eigen::ArrayXXcf _trecv_ = single_pulse.block(0, tt * length, RXnum, length);
      Eigen::ArrayXXcf _pulsecompressed_ = FMCW_PulseCompression(TxSignal, _trecv_, FFTN);
      //
      res.block(0, tt * PulseNum * FFTN + pp * FFTN, RXnum, FFTN) = _pulsecompressed_;
    }
  }
  
  return res;
}