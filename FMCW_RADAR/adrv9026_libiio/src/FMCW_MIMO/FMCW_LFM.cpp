#include "../../include/FMCW_MIMO/FMCW_LFM.h"
#include "../../include/Utilize/common_define.h"

void MIMO_LFM_configure(MIMO_LFM_conf & conf){
  conf.B             = 245e6; // 100 MHz
  conf.fs            = 245760000; // 122e6; // fixed for adrv9025
	conf.PRI           = 12.5e-6;
	conf.tp            = 10.0e-6;
	conf.K             = conf.B / conf.tp;
	conf.PulseNum      = _Calculated_Pulse_Number_;
	conf.PulseLength   = conf.fs * conf.PRI;
  conf.SignalLength  = conf.fs * conf.tp;
  conf.FMCW_wave     = Eigen::ArrayXcf::Zero(conf.PulseLength);
}

Eigen::ArrayXXcf MIMO_LFM(int channel_num, MIMO_LFM_conf & conf){
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
  
  std::cout << "K: " << K << " fs: " << fs << std::endl;
  
  Eigen::ArrayXcf lfm(PRI_num);
  Eigen::ArrayXf t_ = Eigen::ArrayXf::LinSpaced(tp_num, -tp / 2.0, tp / 2.0);
  for (int ii = 0; ii < tp_num; ii++) {
		 lfm(ii) = std::complex<float>{ float(cos(EIGEN_PI * K * t_(ii) * t_(ii))),
										float(sin(EIGEN_PI * K * t_(ii) * t_(ii))) };
	}
  conf.FMCW_wave      = lfm;
  conf.FMCW_wave_conj = lfm.conjugate();
  conf.FFT_FMCW       = Get_FMCW_FFT(lfm, 8192);
 
  for (int ii = 0; ii < channel_num; ii++) {
    for (int jj = 0; jj < tp_num; jj++){
      waveform(ii, ii * PRI_num + jj) = lfm(jj);
    }
  }
 
  return waveform;
}

Eigen::ArrayXXcf MIMO_LFM(int channel_num, bool ec[4], MIMO_LFM_conf & conf) {
  float tp=conf.tp, PRI=conf.PRI, B=conf.B, fs=conf.fs;
  int PRI_num      = PRI * fs,
      tp_num       = tp * fs;
  int single_pulse = PRI_num / channel_num;
  float ts         = 1.0 / fs; 
  // enabled channels
  int echans       = ec[0] + ec[1] + ec[2] + ec[3];

  Eigen::ArrayXXcf waveform = Eigen::ArrayXXcf::Zero(channel_num, echans * PRI_num);
  float K = B / tp;
  if (tp > PRI || tp < 0.0 || PRI < 0.0) {
		assert(!"LFM_waveform: Parameter tp or PRI is fault.\n");
	}
  
  std::cout << "K: " << K << " fs: " << fs << std::endl;
  
  Eigen::ArrayXcf lfm(PRI_num);
  Eigen::ArrayXf t_ = Eigen::ArrayXf::LinSpaced(tp_num, -tp / 2.0, tp / 2.0);
  for (int ii = 0; ii < tp_num; ii++) {
		 lfm(ii) = std::complex<float>{ float(cos(EIGEN_PI * K * t_(ii) * t_(ii))),
										float(sin(EIGEN_PI * K * t_(ii) * t_(ii))) };
	}
  conf.FMCW_wave      = lfm;
  conf.FMCW_wave_conj = lfm.conjugate();
  conf.FFT_FMCW       = Get_FMCW_FFT(lfm, 8192);
 
  int index = 0;
  for (int ii = 0; ii < channel_num; ii++) {
    if (ec[ii]) { 
      for (int jj = 0; jj < tp_num; jj++){
        waveform(ii, index * PRI_num + jj) = lfm(jj);
      }
      index++;
    }
    else { 
      
    }
  }
 
  return waveform;
}