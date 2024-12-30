#include "../../include/Algorithms/PulseDetection.h"

int PulseDetection(
	Eigen::ArrayXXcf recvEcho, 
	int PulseNum,    // ������Ŀ
	int PulseLength, // ���峤��
	int additional, // ���������
	int tpLength) {
	if (recvEcho.cols() < (PulseNum + additional) * PulseLength) {
		assert(!"PulseDetection: Additional Samples are required");
	}
	std::vector<int> peaks_1, peaks_0, delta_1, delta_n1;
	int N_zeros = PulseLength - tpLength;
	float max_peaks = recvEcho.row(0).abs().maxCoeff();
	float detection_threshold = max_peaks * 0.6666666666666667;

	// ���������ʼ��
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
        delta_1.push_back(ii-1);  // ǰ��0 ������1
			else 
        delta_n1.push_back(ii-1); // ǰ��1 ������0
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
	if (peaks_1[0] == 1) { // ��һ���Ǹ������ʱ��
		int first_index = delta_n1[0];
		pulse_start = first_index + N_zeros;
	}
	else { // ��һ���ǵ������ʱ��
		int first_index = delta_1[0];
		pulse_start = first_index + N_zeros + tpLength;
	}
 
	// return recvEcho.block(pulse_start, 0, PulseNum * PulseLength, 1);
	return pulse_start;
}