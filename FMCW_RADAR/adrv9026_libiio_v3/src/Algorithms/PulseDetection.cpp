#include "../../include/Algorithms/PulseDetection.h"

int PulseDetection(
	Eigen::ArrayXXcf recvEcho, 
	int PulseLength,
	int tpLength) {
	std::vector<int> peaks_1, peaks_0, delta_1, delta_n1;
	int N_zeros = PulseLength - tpLength;
	float max_peaks = recvEcho.row(0).abs().maxCoeff();
	float detection_threshold = std::max(max_peaks * 0.85, 150.0); // find the peaks RX1 related to TX1

	for (int ii = 0; ii < recvEcho.cols(); ii++) {
		if (ii > 0) {
			int backward_peaks = std::abs(recvEcho(0, ii - 1)) > detection_threshold;
			int forward_peaks = std::abs(recvEcho(0, ii)) > detection_threshold;
			int delta_peaks = forward_peaks - backward_peaks;
			if (forward_peaks == 1) 
        peaks_1.push_back(ii);
			else 
        peaks_0.push_back(ii);
			if (delta_peaks == 1) 
        delta_1.push_back(ii-1);  // 
			else 
        delta_n1.push_back(ii-1); // 
		}
		else {
			int forward_peaks = std::abs(recvEcho(0, ii)) > detection_threshold;
			if (forward_peaks == 1)
         peaks_1.push_back(ii);
			else 
         peaks_0.push_back(ii);
		}
   
		if (peaks_0.size() >= 1 && peaks_1.size() >= 1 &&
			delta_n1.size() >= 1 && delta_1.size() >= 1)
			break;
	}
 
  if (peaks_0.size() <= 0 || peaks_1.size() <= 0 || 
      delta_n1.size() <= 0 || delta_1.size() <= 0){
      assert(!"PulseDetection: No Valid Pulse is received");
  }
 
	int pulse_start = -1;
	if (peaks_1[0] == 1) { // 
		int first_index = delta_n1[0];
		pulse_start = first_index + N_zeros - 7;
	}
	else { // 
		int first_index = delta_1[0];
		pulse_start = first_index - 7;
	}
 
	// return recvEcho.block(pulse_start, 0, PulseNum * PulseLength, 1);
	return pulse_start;
}

int PulseDetectionSinglePulse(Eigen::ArrayXXcf recvEcho){
  int channels = recvEcho.rows(), samples = recvEcho.cols();
	float max_peaks = recvEcho.abs().maxCoeff();
  float mean_peaks = recvEcho.abs().mean();
	float detection_threshold = std::max(max_peaks * 0.3, 50.0);
  
  int index = 0;
  for (int cc = 0; cc < channels; cc++){
    for (int ii = 0; ii < samples - 7; ii++){
      int val = 0;
      for (int jj = 0; jj < 7; jj++){
        if (abs(recvEcho(cc, ii + jj)) > detection_threshold){
          val++;
        }
      }
      if (val >= 2) {
        index = ii;
        break;
      }
    }
  }
  
  return index;
}

int PulseDetection_LowPulse(Eigen::ArrayXXcf recvEcho, int enable_chan, int signal_length) {
  // find the first pulse
  int pulse_index = 4;
  float noise = 500.0;
  for (; pulse_index < signal_length * 4 - 1; pulse_index++) {
    bool cond1  = abs(abs(recvEcho(0, pulse_index)) - abs(recvEcho(0, pulse_index - 1))) > noise;
    bool cond2  = abs(abs(recvEcho(0, pulse_index)) - abs(recvEcho(0, pulse_index - 2))) > noise;
    bool cond3  = abs(abs(recvEcho(0, pulse_index)) - abs(recvEcho(0, pulse_index - 3))) > noise;
    bool cond4  = abs(abs(recvEcho(0, pulse_index)) - abs(recvEcho(0, pulse_index - 4))) > noise;
    bool cond0  = abs(recvEcho(0, pulse_index + 1)) > noise;
    bool cond5  = abs(recvEcho(0, pulse_index)) > noise;
    bool cond6  = abs(recvEcho(0, pulse_index - 1)) < noise;
    bool cond7  = abs(recvEcho(0, pulse_index - 2)) < noise;
    bool cond8  = abs(recvEcho(0, pulse_index - 3)) < noise;
    bool cond9  = abs(recvEcho(0, pulse_index - 4)) < noise;
    bool cond10 = abs(abs(recvEcho(0, pulse_index-1)) - abs(recvEcho(0, pulse_index-2))) < noise;
    bool cond11 = abs(abs(recvEcho(0, pulse_index-1)) - abs(recvEcho(0, pulse_index-3))) < noise;
    bool cond12 = abs(abs(recvEcho(0, pulse_index-1)) - abs(recvEcho(0, pulse_index-4))) < noise;
    bool cond13 = abs(abs(recvEcho(0, pulse_index-2)) - abs(recvEcho(0, pulse_index-3))) < noise;
    bool cond14 = abs(abs(recvEcho(0, pulse_index-2)) - abs(recvEcho(0, pulse_index-4))) < noise;
    bool cond15 = abs(abs(recvEcho(0, pulse_index-3)) - abs(recvEcho(0, pulse_index-4))) < noise;
    int conds   = cond1 + cond2 + cond3 + cond4 + cond5 + cond6 + cond7 + cond8 + cond9 +
                  cond10 + cond11 + cond12 + cond13 + cond14 + cond15;
    if (conds >= 12) break;
  }
  
  // find the after 4 channels
  int max_index = -1; float max_value = -1;
  for (int ii = 0; ii < enable_chan; ii++) {
    float _comp_ = 0;
    for (int iii = 0; iii < 5; iii++)
      _comp_ += abs(recvEcho(0, signal_length * ii + pulse_index + iii));
    if (_comp_ > max_value) {
      max_index = signal_length * ii + pulse_index;
      max_value = _comp_;
    }
  }
  
  // provent overlength
  if (max_index > signal_length * 2) {
    max_index -= int(max_index / signal_length) * signal_length;
  }
  
  return max_index >= 0? max_index: 0;
}

int PulseDetection_LowPulse(iio_buffer * rx_buf, int loop_chan, int signal_length, int enable_chans) {
  int pulse_index = 4 * enable_chans * sizeof(std::complex<short>), // (IQ0 4 IQ1 8 IQ2 12 IQ3 16) * 4
    byte_interval = enable_chans * sizeof(std::complex<short>), // 16
    loop_interval = loop_chan * sizeof(std::complex<short>); // 0
  float noise = 500.0;
  
  // get the address
  char * start_address = (char *)iio_buffer_start(rx_buf);
  
  for (;pulse_index < signal_length * byte_interval * 4; pulse_index += byte_interval) { 
    short real_4, real_3, real_2, real_1, real_0, real_p1;
    short imag_4, imag_3, imag_2, imag_1, imag_0, imag_p1;
    
    std::memcpy(&real_4, start_address +  pulse_index - 4 * byte_interval + loop_interval, 2);
    std::memcpy(&imag_4, start_address +  pulse_index - 4 * byte_interval + loop_interval + 2, 2);
    std::memcpy(&real_3, start_address +  pulse_index - 3 * byte_interval + loop_interval, 2);
    std::memcpy(&imag_3, start_address +  pulse_index - 3 * byte_interval + loop_interval + 2, 2);
    std::memcpy(&real_2, start_address +  pulse_index - 2 * byte_interval + loop_interval, 2);
    std::memcpy(&imag_2, start_address +  pulse_index - 2 * byte_interval + loop_interval + 2, 2);
    std::memcpy(&real_1, start_address +  pulse_index - 1 * byte_interval + loop_interval, 2);
    std::memcpy(&imag_1, start_address +  pulse_index - 1 * byte_interval + loop_interval + 2, 2);
    std::memcpy(&real_0, start_address +  pulse_index - 0 * byte_interval + loop_interval, 2);
    std::memcpy(&imag_0, start_address +  pulse_index - 0 * byte_interval + loop_interval + 2, 2);
    std::memcpy(&real_p1, start_address + pulse_index + 1 * byte_interval + loop_interval, 2);
    std::memcpy(&imag_p1, start_address + pulse_index + 1 * byte_interval + loop_interval + 2, 2);
    
    std::complex <float> data1 = std::complex<float> {real_1, imag_1},
     data2 = std::complex<float> {real_2, imag_2},
     data3 = std::complex<float> {real_3, imag_3},
     data4 = std::complex<float> {real_4, imag_4},
     data0 = std::complex<float> {real_0, imag_0},
     datap1 = std::complex<float> {real_p1, imag_p1};
     
    bool cond1  = abs(abs(data0) - abs(data1)) > noise;
    bool cond2  = abs(abs(data0) - abs(data2)) > noise;
    bool cond3  = abs(abs(data0) - abs(data3)) > noise;
    bool cond4  = abs(abs(data0) - abs(data4)) > noise;
    bool cond0  = abs(datap1) > noise;
    bool cond5  = abs(data0) > noise;
    bool cond6  = abs(data1) < noise;
    bool cond7  = abs(data2) < noise;
    bool cond8  = abs(data3) < noise;
    bool cond9  = abs(data4) < noise;
    bool cond10 = abs(abs(data1) - abs(data2)) < noise;
    bool cond11 = abs(abs(data1) - abs(data3)) < noise;
    bool cond12 = abs(abs(data1) - abs(data3)) < noise;
    bool cond13 = abs(abs(data2) - abs(data3)) < noise;
    bool cond14 = abs(abs(data2) - abs(data4)) < noise;
    bool cond15 = abs(abs(data3) - abs(data4)) < noise;
    int conds   = cond1 + cond2 + cond3 + cond4 + cond5 + cond6 + cond7 + cond8 + cond9 +
                  cond10 + cond11 + cond12 + cond13 + cond14 + cond15;
                  
    if (conds >= 12) break;
  }
  
  return pulse_index;
}