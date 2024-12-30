#include "../../include/FMCW_MIMO/FMCW_LFM.h"

void MIMO_LFM_configure(MIMO_LFM_conf & conf){
  conf.B = 200e6; // 100 MHz
  conf.fs = 245760000; // 122e6; // fixed for adrv9025
	conf.PRI = 150e-6;
	conf.tp = 145e-6;
  conf.info = 10e-6; // 信息脉冲长度
	conf.K = conf.B / conf.tp;
	conf.PulseNum = 1.333333333333333333333333334;
	conf.PulseLength = conf.fs * conf.PRI;
  conf.SignalLength = conf.fs * conf.tp;
}

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf conf){
  float tp=conf.tp, PRI=conf.PRI, B=conf.B, fs=conf.fs;
  int PRI_num  = PRI * fs,
      tp_num   = tp * fs;
  int single_pulse = PRI_num / channel_num;
  float ts    = 1.0 / fs; 

  Eigen::ArrayXXcf waveform = Eigen::ArrayXXcf::Zero(channel_num, channel_num * PRI_num);
  float K = B / tp;
  if (tp > PRI || tp < 0.0 || PRI < 0.0) {
		assert(!"LFM_waveform: Parameter tp or PRI is fault.\n");
	}
  
  Eigen::ArrayXcf lfm(PRI_num);
  Eigen::ArrayXf t_ = Eigen::ArrayXf::LinSpaced(tp_num, -tp / 2.0, tp / 2.0);
  for (int ii = 0; ii < tp_num; ii++) {
		lfm(ii) = std::complex<float>{ float(cos(EIGEN_PI * K * t_(ii) * t_(ii))),
										float(sin(EIGEN_PI * K * t_(ii) * t_(ii))) };
	}
 
  for (int ii = 0; ii < channel_num; ii++) {
    for (int jj = 0; jj < tp_num; jj++){
      waveform(ii, ii * PRI_num + jj) = lfm(jj);
    }
  }
 
  return waveform;
}