#ifndef _COMMON_DEFINE_H_
#define _COMMON_DEFINE_H_

#include <iio.h>
#include <Eigen/Core>
#include <Eigen/Dense>
// 常见定义

/* RX is input, TX is output */
enum iodev { RX, TX, PHY };

/* common RX and TX streaming params */
struct stream_cfg {
	long long lo_hz; // Local oscillator frequency in Hz
	long long fs_hz; // sample rate in Hz
	long long bw_hz; // analog bandwidth in Hz
};

// Signal Define
#define _Calculated_Pulse_Number_ 64 // For RX
#define _Expected_TX_Pulse_Number_ 1 // ** if use the cyclic buffer must be 1 **
#define _Use_Reference_Chan_       1 // Reference Channel Enabled (default 1)
#define _Reference_Chan_           0 // Reference Loop Back Channel (default 0)

// 通道数启用情况
#define _ENABLE_CH1_TX_ 1
#define _ENABLE_CH2_TX_ 1
#define _ENABLE_CH3_TX_ 1
#define _ENABLE_CH4_TX_ 1
#define _ENABLE_CH1_RX_ 1
#define _ENABLE_CH2_RX_ 1
#define _ENABLE_CH3_RX_ 1
#define _ENABLE_CH4_RX_ 1
// 一些定义
#define _ARRAY_NUM_  4
#define _LO_         5.0e9
#define _2_13_       8191.0
#define _DMA_Number_ 4

// Include
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <chrono>

#define _USE_ADRV9025_ 1
#define _USE_ADRV9009_ 0

// TX use Cylic Buffer 
#define _USE_CYLIC_BUFFER_ 1

// FFT Number
#define _RPFFT_NUM_ 4096
#define _DPFFT_NUM_ 256
#define _RFFT_NUM_ 2048
#define _DFFT_NUM_ 256

/* helper macros */
#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

#define IIO_ENSURE(expr) { \
	if (!(expr)) { \
		(void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
		(void) abort(); \
	} \
}

void Send_TX(iio_buffer * txbuf, int16_t * _data_, bool & new_data);

void Recv_RX(iio_buffer * rxbuf, int16_t * _data_rx_, bool & new_data);

/* cleanup and exit */
void shutdown(void);

void handle_sig(int sig);

/* check return value of attr_write function */
void errchk(int v, const char* what);

/* write attribute: long long int */
void wr_ch_lli(struct iio_channel *chn, const char* what, long long val);

void wr_ch_b(struct iio_channel *chn, const char* what, bool val);

void wr_ch_d(struct iio_channel *chn, const char* what, double val);

/* write attribute: long long int */
long long rd_ch_lli(struct iio_channel *chn, const char* what);

/* write attribute: string */
void wr_ch_str(struct iio_channel *chn, const char* what, const char* str);

/* helper function generating channel names */
char* get_ch_name_mod(const char* type, int id, char modify);

/* helper function generating channel names */
char* get_ch_name(const char* type, int id);

// COMMON DEFINE PARAMETERS
Eigen::MatrixXcf GetDFTMatrix(int points);

#endif