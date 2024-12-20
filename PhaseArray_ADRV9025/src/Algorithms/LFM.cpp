#include "../../include/Algorithms/LFM.h"

// ��ʼ��LFM�ź�����
// Ĭ�ϲ���
void LFM_configure(LFM_conf& conf) {
	conf.B = 50e6; // 100 MHz
	conf.fs = 245760000; // 122e6; fixed for adrv9025
	conf.PRI = 50e-6;
	conf.tp = 20e-6;
	conf.K = conf.B / conf.tp;
	conf.PulseNum = 64;
	conf.PulseLength = conf.fs * conf.PRI;
  conf.SignalLength = conf.fs * conf.tp;
}

// ����LFM�ź�
// ����1��fs ������ (240MHz)
// ����2��B ����
// ����3��PRI �����ظ�����
// ����4��tp ���� (us)
// ���1��LFM�ź�
Eigen::ArrayXcf LFM_waveform(float fs, float B, float PRI, float tp) {
	unsigned int pulse_length = int(PRI * fs); // ���峤��
	unsigned int tp_length = int(tp * fs); // �źų���
	float K = B / tp,// ��Ƶб��
		ts = 1.0 / fs; 
	Eigen::ArrayXcf lfm = Eigen::ArrayXcf::Zero(pulse_length);

	if (tp > PRI || tp < 0.0 || PRI < 0.0) {
		assert(!"LFM_waveform: Parameter tp or PRI is fault.\n");
	}
  
	for (int ii = 0; ii < tp_length; ii++) {
		lfm(ii) = std::complex<float>{ float(cos(B / 2.0 - EIGEN_PI * K * ii * ii * ts * ts)),
										float(sin(B / 2.0 - EIGEN_PI * K * ii * ii * ts * ts)) };
	}
	return lfm;
}

// ����ƥ���˲���
// ����1���ź�
// ���1��ƥ���˲���Ľ��
Eigen::ArrayXcf LFM_matchfilter(Eigen::ArrayXcf signal) {
	unsigned int signal_length = signal.rows();

	if (signal_length == 0) {
		assert(!"LFM MatchFilter: Signal length should not be zero");
	}

	fftw_complex* din = NULL, * out = NULL;
	fftw_plan p;
	din = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * signal.size());
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * signal.size());
	Eigen::ArrayXcf _match_filter_ = Eigen::ArrayXcf::Zero(signal_length);
	if (din == NULL || out == NULL) {
		assert(!"LFM_matchfilter: Din or Out malloc failure!\n");
	}

	for (int ii = 0; ii < signal_length; ii++) {
		din[ii][0] = signal(ii).real();
		din[ii][1] = -signal(ii).imag();
	}

	p = fftw_plan_dft_1d(signal_length, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p); /* repeat as needed */
	fftw_destroy_plan(p);
	fftw_cleanup();

	for (int ii = 0; ii < signal_length; ii++) {
		_match_filter_(ii) = std::complex<float>{
			float(out[ii][0]), float(out[ii][1])
		};
	}

	fftw_free(din);
	fftw_free(out);

	return _match_filter_;
}

// ����Ҷ�任
// ����1����Ҫ������Ҷ�任������
Eigen::ArrayXcf LFM_fft(Eigen::ArrayXcf Data) {
	unsigned int data_length = Data.size();
	fftw_complex* din = NULL, * out = NULL;
	fftw_plan p;
	din = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Data.size());
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Data.size());
	Eigen::ArrayXcf _data_(data_length);
	if (din == NULL || out == NULL) {
		assert(!"LFM_fft: Din or Out malloc failure!\n");
	}

	for (int ii = 0; ii < data_length; ii++) {
		din[ii][0] = Data(ii).real();
		din[ii][1] = Data(ii).imag();
	}

	p = fftw_plan_dft_1d(data_length, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p); /* repeat as needed */
	fftw_destroy_plan(p);
	fftw_cleanup();

	for (int ii = 0; ii < data_length; ii++) {
		_data_(ii) = std::complex<float>{
			float(out[ii][0]), float(out[ii][1])
		};
	}

	fftw_free(din);
	fftw_free(out);
	return _data_;
}

// �渵��Ҷ�任
// ����1����Ҫ���渵��Ҷ�任������
Eigen::ArrayXcf LFM_ifft(Eigen::ArrayXcf Data) {
	unsigned int data_length = Data.size();
	fftw_complex* din = NULL, * out = NULL;
	fftw_plan p;
	din = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Data.size());
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Data.size());
	Eigen::ArrayXcf _data_(data_length);
	if (din == NULL || out == NULL) {
		assert(!"LFM_ifft: Din or Out malloc failure!\n");
	}

	for (int ii = 0; ii < data_length; ii++) {
		din[ii][0] = Data(ii).real();
		din[ii][1] = Data(ii).imag();
	}

	p = fftw_plan_dft_1d(data_length, din, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	fftw_cleanup();

	for (int ii = 0; ii < data_length; ii++) {
		_data_(ii) = std::complex<float>{
			float(out[ii][0]) / data_length,
			float(out[ii][1]) / data_length
		};
	}

	fftw_free(din);
	fftw_free(out);
	return _data_;
}

// ����ѹ��
// ����1���ز�
// ����2��ƥ���˲���
// ����3��������Ŀ by default it's 128
// ����4�����峤��
// ���1������ѹ����ز�
Eigen::ArrayXcf LFM_pulsecompression(Eigen::ArrayXcf Echo,
	Eigen::ArrayXcf matchfilter,
	unsigned int pulse_num,
	unsigned int pulse_length) {
	Eigen::ArrayXcf pulse_compressed(pulse_num * pulse_length);
	for (int ii = 0; ii < pulse_num; ii++) {
		Eigen::ArrayXcf pulse_part = 
			Echo.block(0 + (ii - 1) * pulse_length, 0, pulse_length, 1);
		// ȡ������
		Eigen::ArrayXcf fft_pulse = LFM_fft(pulse_part);
		// Ԫ�����
		Eigen::ArrayXcf multi = fft_pulse * matchfilter;
		// �õ���ѹ���
		Eigen::ArrayXcf pulse_comp = LFM_ifft(multi);
		// ���ص�������
		pulse_compressed.block(0 + (ii - 1) * pulse_length, 0, pulse_length, 1) =
			pulse_comp;
	}
	return pulse_compressed;
}

// ��Ŀ��ָʾ
// ����1���ز�
// ����2��������Ŀ
// ����3�����峤��
// ���1����Ŀ��ָʾ��ز�
Eigen::ArrayXcf LFM_MTI(Eigen::ArrayXcf Echo,
	unsigned int pulse_num,
	unsigned int pulse_length) {
	unsigned int expected_length = (pulse_num - 1) * pulse_length;
	Eigen::ArrayXcf expected_fix = Echo.block(pulse_length-1, 0, expected_length, 1),
		expected_sub = Echo.block(0, 0, expected_length, 1);
	return expected_fix - expected_sub;
}

// ��Ŀ����
// ����1���ز�
// ����2��������Ŀ
// ����3�����峤��
// ���1������-��������
Eigen::MatrixXcf LFM_MTD(Eigen::ArrayXcf Echo,
						unsigned int pulse_num,
						unsigned int pulse_length) {
	Eigen::MatrixXcf mtd(pulse_num, pulse_length);

	for (int ii = 0; ii < pulse_length; ii++) {
		Eigen::ArrayXcf corr(pulse_num);
		for (int jj = 0; jj < pulse_num; jj++) {
			corr(jj) = std::complex<float>{
				Echo(jj * pulse_length + ii).real(),
				Echo(jj * pulse_length + ii).imag(),
			};
		}
		Eigen::ArrayXcf velo = LFM_fft(corr);
		// FFT
		for (int jj = 0; jj < pulse_num; jj++) {
			mtd(ii, jj) = std::complex<float>{
				velo(jj * pulse_length + ii).real(),
				velo(jj * pulse_length + ii).imag(),
			};
		}
	}

	return mtd;
}
