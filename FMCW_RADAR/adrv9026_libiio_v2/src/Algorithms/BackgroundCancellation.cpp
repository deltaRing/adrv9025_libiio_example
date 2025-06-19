#include "../../include/Algorithms/BackgroundCancellation.h"

Eigen::MatrixXcf GetDFTBack(int points) {
  Eigen::MatrixXcf DFT(points, points);
  
  for (int k = 0; k < points; ++k) {
    for (int n = 0; n < points; ++n) {
      float angle = -2.0 * 3.1415926535 * k * n / points;
      DFT(k, n) = std::complex<float>{ cos(angle), sin(angle) };
    }
  }
  
  return DFT;
}

Eigen::MatrixXcf GetIDFTBack(int points) {
  Eigen::MatrixXcf IDFT(points, points);
  
  for (int k = 0; k < points; ++k) {
    for (int n = 0; n < points; ++n) {
      float angle = 2.0 * 3.1415926535 * k * n / points;
      IDFT(k, n) = std::complex<float>{ cos(angle) / points, sin(angle) / points };
    }
  }
  
  return IDFT;
}

// Params
Eigen::MatrixXcf DFTm = GetDFTBack(24);
Eigen::MatrixXcf iDFTm = GetIDFTBack(24);

std::vector<int> desire_index = {490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508,
                                   530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 548};
std::vector<int> cancel_time = {8, 8, 8, 16, 16, 16, 16, 24, 24, 24, 24, 24, 16, 16, 16, 16, 8, 8, 8,
                                  8, 8, 8, 16, 16, 16, 16, 24, 24, 24, 24, 24, 16, 16, 16, 16, 8, 8, 8};
float cancel_weight = 8.0;
bool update_param = false;
extern CancellationParam cparams;

void GetRegularIntermSignal(Eigen::ArrayXXcf & qInterm, 
                            Eigen::ArrayXXcf & qSignal,
                            MIMO_LFM_conf config,
                            int start_index) {
  Eigen::ArrayXXcf ref_signal1 = Eigen::ArrayXXcf::Zero(1, config.PulseLength),
                  cap_signal1 = Eigen::ArrayXXcf::Ones(1, config.PulseLength),
                  ref_signals = Eigen::ArrayXXcf::Zero(1, config.PulseLength * int(config.PulseNum)),
                  cap_signals = Eigen::ArrayXXcf::Zero(1, config.PulseLength * int(config.PulseNum));
  
  for (int ii = 0; ii < int(config.PulseNum); ii++) {
    ref_signal1 = qSignal.block(0, start_index + ii * config.PulseLength, 1, config.PulseLength);
    cap_signal1.block(0, 0, 1, config.SignalLength) = ref_signal1.abs().maxCoeff() /
    ref_signal1.block(0, 0, 1, config.SignalLength);
     
    ref_signals.block(0, ii * config.PulseLength, 1, config.PulseLength) = cap_signal1 * ref_signal1;
    // chan 1
    
    ref_signal1 = qSignal.block(1, start_index + ii * config.PulseLength, 1, config.PulseLength);
    cap_signal1.block(0, 0, 1, config.SignalLength) = ref_signal1.abs().maxCoeff() /
     ref_signal1.block(0, 0, 1, config.SignalLength);
    cap_signals.block(0, ii * config.PulseLength, 1, config.PulseLength) = cap_signal1 * ref_signal1;
  }
  
  qInterm = ref_signals * ref_signals.conjugate();
}

void UpdateParams(Eigen::ArrayXXcf range_profile, CancellationParam params) {
  desire_index.clear();
  cancel_time.clear();
  
  printf("*** Updating Cancellation Parameters ***\n");
  printf("*** Cancellation Params expand_num: %d ***\n", params.expand_num);
  printf("*** Cancellation Params init_cancel_num: %d ***\n", params.init_cancel_num);
  printf("*** Cancellation Params separate_num: %d ***\n", params.separate_num);
  printf("*** Cancellation Params cancel_weight: %f ***\n", params.cancel_weight);
  
  int expected = 2;
  std::vector<int> center_index;
  while (1) {
    if (center_index.size() >= expected) 
      break;
    
    Eigen::Index row, col;
    float maxVal = range_profile.row(0).abs().maxCoeff(&row, &col);
    range_profile(row, col) = std::complex<float>{0.0, 0.0};
    bool find = false;
    
    if (center_index.size() == 0)
      center_index.push_back(col);
    else {
      for (int jj = 0; jj < center_index.size(); jj++) {
        if (abs(col - center_index[jj]) < cparams.separate_num) {
          find = true;
          break;
        }
      }
      center_index.push_back(col);
    }
    
    if (find) continue;
    
    std::cout << " *** Find Index " << col << " *** \n";
    
    for (int jj = -params.expand_num; jj < params.expand_num; jj++) {
      if (abs(jj) > params.expand_num / 3 * 2) {
        cancel_time.push_back(params.init_cancel_num);
      }else if (abs(jj) <= params.expand_num / 3 * 2 && abs(jj) > params.expand_num / 4) {
        cancel_time.push_back(params.init_cancel_num * 2);
      }else {
        cancel_time.push_back(params.init_cancel_num * 4);
      }
      desire_index.push_back(jj + col);
    }
    
    std::cout << "Detect Index: " << col << std::endl;
  }
  
  std::cout << "Detect the Desire Index: " << std::endl;
  for (int jj = 0; jj < desire_index.size(); jj++) {
    std::cout << desire_index[jj] << " ";
  }
  std::cout << std::endl;
  
  std::cout << "Using Cancellation Time: " << std::endl;
  for (int jj = 0; jj < cancel_time.size(); jj++) {
    std::cout << cancel_time[jj] << " ";
  }
  std::cout << std::endl;
}

void RangeProfileCancellation(Eigen::ArrayXXcf & range_profile) {
  if (!update_param) {
    DFTm = GetDFTBack(range_profile.rows());
    iDFTm = GetIDFTBack(range_profile.rows());
    UpdateParams(range_profile, cparams);
    update_param = true;
  }
  
  if (desire_index.size() != cancel_time.size()) {
    std::cout << "Range Profile Cancellation: Desire Index and Cancel Time not Equal\n";
    return;
  }
  
  /*std::ofstream real("rr.txt"), imag("ii.txt");
  real << range_profile.real();
  imag << range_profile.imag();
  real.close();
  imag.close();*/
  
  for (int ii = 0; ii < desire_index.size(); ii++) {
    Eigen::ArrayXXcf fft_desire = range_profile.col(desire_index[ii]);
    Eigen::ArrayXXcf fft_result = DFTm * fft_desire.matrix();
    for (int jj = 0; jj < cancel_time[ii]; jj++) {
      Eigen::Index row, col;
      float maxVal = fft_result.abs().maxCoeff(&row, &col);
      fft_result(row, col) = fft_result(row, col) / cparams.cancel_weight;
    }
    Eigen::ArrayXXcf ifft_result = iDFTm * fft_result.matrix();
    range_profile.col(desire_index[ii]) = ifft_result;
  }
  
  Eigen::ArrayXXcf background = Eigen::ArrayXXcf::Zero(1, range_profile.cols());
  for (int ii = 0; ii < range_profile.rows(); ii++) {
    background = background + range_profile.row(ii);
  }
  
  /*real.open("gr.txt"), imag.open("gi.txt");
  real << background.real();
  imag << background.imag();
  real.close();
  imag.close();*/
  
  for (int ii = 0; ii < range_profile.rows(); ii++) {
    range_profile.row(ii) -= background;
  }
  
  /*real.open("brr.txt"), imag.open("bii.txt");
  real << range_profile.real();
  imag << range_profile.imag();
  real.close();
  imag.close();*/
}
