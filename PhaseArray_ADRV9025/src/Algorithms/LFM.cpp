#include "../../include/Algorithms/LFM.h"

// 初始化LFM信号配置
// 默认参数
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

// 生成LFM信号
// 输入1：fs 采样率 (240MHz)
// 输入2：B 带宽
// 输入3：PRI 脉冲重复周期
// 输入4：tp 脉宽 (us)
// 输出1：LFM信号
Eigen::ArrayXcf LFM_waveform(float fs, float B, float PRI, float tp) {
	unsigned int pulse_length = int(PRI * fs); // 脉冲长度
	unsigned int tp_length = int(tp * fs); // 信号长度
	float K = B / tp,// 调频斜率
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

// 生成匹配滤波器
// 输入1：信号
// 输出1：匹配滤波后的结果
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

// 傅里叶变换
// 输入1：想要做傅里叶变换的数据
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

// 逆傅里叶变换
// 输入1：想要做逆傅里叶变换的数据
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

// 脉冲压缩
// 输入1：回波
// 输入2：匹配滤波器
// 输入3：脉冲数目 by default it's 128
// 输入4：脉冲长度
// 输出1：脉冲压缩后回波
Eigen::ArrayXcf LFM_pulsecompression(Eigen::ArrayXcf Echo,
	Eigen::ArrayXcf matchfilter,
	unsigned int pulse_num,
	unsigned int pulse_length) {
	Eigen::ArrayXcf pulse_compressed(pulse_num * pulse_length);
	for (int ii = 0; ii < pulse_num; ii++) {
		Eigen::ArrayXcf pulse_part = 
			Echo.block(0 + (ii - 1) * pulse_length, 0, pulse_length, 1);
		// 取出波形
		Eigen::ArrayXcf fft_pulse = LFM_fft(pulse_part);
		// 元素相乘
		Eigen::ArrayXcf multi = fft_pulse * matchfilter;
		// 得到脉压结果
		Eigen::ArrayXcf pulse_comp = LFM_ifft(multi);
		// 返回到矩阵中
		pulse_compressed.block(0 + (ii - 1) * pulse_length, 0, pulse_length, 1) =
			pulse_comp;
	}
	return pulse_compressed;
}

// 动目标指示
// 输入1：回波
// 输入2：脉冲数目
// 输入3：脉冲长度
// 输出1：动目标指示后回波
Eigen::ArrayXcf LFM_MTI(Eigen::ArrayXcf Echo,
	unsigned int pulse_num,
	unsigned int pulse_length) {
	unsigned int expected_length = (pulse_num - 1) * pulse_length;
	Eigen::ArrayXcf expected_fix = Echo.block(pulse_length-1, 0, expected_length, 1),
		expected_sub = Echo.block(0, 0, expected_length, 1);
	return expected_fix - expected_sub;
}

// 动目标检测
// 输入1：回波
// 输入2：脉冲数目
// 输入3：脉冲长度
// 输出1：距离-多普勒谱
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
