// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * libiio - ADRV9025 IIO streaming example
 *
 * Copyright (C) 2014 IABG mbH
 * Author: Michael Feilen <feilen_at_iabg.de>
 * Copyright (C) 2019 Analog Devices Inc.
 **/

#include "include/Algorithms/LFM.h"
#include "include/Algorithms/Beamforming.h"
#include "include/Algorithms/PulseDetection.h"
#include "include/Utilize/test_signal.h"
#include "include/Utilize/data_convert.h"
#include "include/Utilize/iiostream-set-attribution.h"
#include <iostream>
#include <fstream>
#include <chrono>

/* static scratch mem for strings */
extern char tmpstr[64];
extern bool stop;

/* IIO structs required for streaming */
extern struct iio_context *ctx;
extern struct iio_device  *tx;
extern struct iio_device  *rx;
extern struct iio_channel *rx0_i;
extern struct iio_channel *rx0_q;
extern struct iio_channel *tx0_i;
extern struct iio_channel *tx0_q;
extern struct iio_channel *rx1_i;
extern struct iio_channel *rx1_q;
extern struct iio_channel *tx1_i;
extern struct iio_channel *tx1_q;
extern struct iio_channel *rx2_i;
extern struct iio_channel *rx2_q;
extern struct iio_channel *tx2_i;
extern struct iio_channel *tx2_q;
extern struct iio_channel *rx3_i;
extern struct iio_channel *rx3_q;
extern struct iio_channel *tx3_i;
extern struct iio_channel *tx3_q;
extern struct iio_buffer  *rxbuf;
extern struct iio_buffer  *txbuf;

#define _ENABLE_CH1_ 1
#define _ENABLE_CH2_ 1
#define _ENABLE_CH3_ 1
#define _ENABLE_CH4_ 1
#define _ARRAY_NUM_  4
#define _2_14_       8192

/* simple configuration and streaming */
int main (__notused int argc, __notused char **argv)
{
  // Initialize Waveform
  LFM_conf conf;
	LFM_configure(conf);
  int signal_length = conf.PulseLength;
  int pulse_number  = (conf.PulseNum + 2) * signal_length;
  
  // Generate Signal
  Eigen::ArrayXcf LFM = LFM_waveform(conf.fs, conf.B, conf.PRI, conf.tp);
  // Generate SteerVector
	Eigen::ArrayXcf SteerVector = LinearSteerVector(_ARRAY_NUM_, -5.0 / 180.0 * EIGEN_PI, 1.8e9, 3e8 / 1.8e9 / 2);

	// RX and TX sample counters
	size_t nrx = 0;
	size_t ntx = 0;

	// RX and TX sample size
	size_t rx_sample_sz, tx_sample_sz;

	// Stream configuration
	struct stream_cfg trxcfg;
	int err;

	// Listen to ctrl+c and IIO_ENSURE
	signal(SIGINT, handle_sig);

	// TRX stream config
	trxcfg.lo_hz = GHZ(2.8);

	printf("* Acquiring IIO context\n");
	IIO_ENSURE((ctx = iio_create_default_context()) && "No context");
	IIO_ENSURE(iio_context_get_devices_count(ctx) > 0 && "No devices");

	printf("* Acquiring ADRV9025 streaming devices\n");
	IIO_ENSURE(get_adrv9025_stream_dev(RX, &rx) && "No rx dev found");
	IIO_ENSURE(get_adrv9025_stream_dev(TX, &tx) && "No tx dev found");
 
   
  printf("* Configuring ADRV9025 for streaming\n");
  if (_ENABLE_CH1_) IIO_ENSURE(cfg_adrv9025_streaming_ch(&trxcfg, 0) && "TRX 0 device not found");
 	if (_ENABLE_CH2_) IIO_ENSURE(cfg_adrv9025_streaming_ch(&trxcfg, 1) && "TRX 1 device not found");
 	if (_ENABLE_CH3_) IIO_ENSURE(cfg_adrv9025_streaming_ch(&trxcfg, 2) && "TRX 2 device not found");
 	if (_ENABLE_CH4_) IIO_ENSURE(cfg_adrv9025_streaming_ch(&trxcfg, 3) && "TRX 3 device not found");
   
  printf("* Initializing ADRV9025 Channels IIO streaming channels\n"); 
  if (_ENABLE_CH1_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 0, 'i', &rx0_i) && "RX chan 1 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 0, 'q', &rx0_q) && "RX chan 1 q not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 0, 'i', &tx0_i) && "TX chan 1 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 0, 'q', &tx0_q) && "TX chan 1 q not found");
  }
 
  if (_ENABLE_CH2_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 1, 'i', &rx1_i) && "RX chan 2 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 1, 'q', &rx1_q) && "RX chan 2 q not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 1, 'i', &tx1_i) && "TX chan 2 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 1, 'q', &tx1_q) && "TX chan 2 q not found");
  }
  
  if (_ENABLE_CH3_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 2, 'i', &rx2_i) && "RX chan 3 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 2, 'q', &rx2_q) && "RX chan 3 q not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 2, 'i', &tx2_i) && "TX chan 3 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 2, 'q', &tx2_q) && "TX chan 3 q not found");
  }
  
  if (_ENABLE_CH4_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 3, 'i', &rx3_i) && "RX chan 4 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 3, 'q', &rx3_q) && "RX chan 4 q not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 3, 'i', &tx3_i) && "TX chan 4 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 3, 'q', &tx3_q) && "TX chan 4 q not found");
  }
 
  printf("* Enabling IIO streaming channels\n");
  if (_ENABLE_CH1_) {
    printf("* Enabling Channel 1\n");
	  iio_channel_enable(rx0_i);
	  iio_channel_enable(rx0_q);
	  iio_channel_enable(tx0_i);
	  iio_channel_enable(tx0_q);
  }
  
  if (_ENABLE_CH2_) {
    printf("* Enabling Channel 2\n");
	  iio_channel_enable(rx1_i);
	  iio_channel_enable(rx1_q);
	  iio_channel_enable(tx1_i);
	  iio_channel_enable(tx1_q);
  }
  
  if (_ENABLE_CH3_) {
    printf("* Enabling Channel 3\n");
	  iio_channel_enable(rx2_i);
	  iio_channel_enable(rx2_q);
	  iio_channel_enable(tx2_i);
	  iio_channel_enable(tx2_q);
  }
  
  if (_ENABLE_CH4_) {
    printf("* Enabling Channel 4\n");
	  iio_channel_enable(rx3_i);
	  iio_channel_enable(rx3_q);
	  iio_channel_enable(tx3_i);
	  iio_channel_enable(tx3_q);
  }
 
	printf("* Creating non-cyclic RX IIO buffers\n");
	rxbuf = iio_device_create_buffer(rx, pulse_number, false);
  if (!rxbuf) {
		perror("Could not create RX buffer");
		shutdown();
 	}
	printf("* Creating cyclic TX IIO buffers\n");
  txbuf = iio_device_create_buffer(tx, signal_length, true);
 	if (!txbuf) {
		perror("Could not create TX buffer");
		shutdown();
 	}
 
	char *p_dat, *p_end;
	ssize_t nbytes_rx, nbytes_tx;
	ptrdiff_t p_inc;
  int ii = 0;
  
  /* WRITE: Get pointers to TX buf and write IQ to TX buf port 0 */
  p_inc = iio_buffer_step(txbuf);
  p_end = (char*)iio_buffer_end(txbuf);
  uint8_t check_sum = _ENABLE_CH1_ + _ENABLE_CH2_ + _ENABLE_CH3_ + _ENABLE_CH4_;
  for (p_dat = (char*)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
    if (check_sum >= 1){
      std::complex<float> data = LFM(ii % signal_length) * SteerVector(0);
      ((int16_t*)(p_dat))[0]   = data.real() * _2_14_; /* Real (I) */
      ((int16_t*)(p_dat))[1]   = data.imag() * _2_14_; /* Imag (Q) */    
    }if (check_sum >= 2){
      std::complex<float> data = LFM(ii % signal_length) * SteerVector(1);
      ((int16_t*)(p_dat))[2]   = data.real() * _2_14_; /* Real (I) */
      ((int16_t*)(p_dat))[3]   = data.imag() * _2_14_; /* Imag (Q) */  
    }if (check_sum >= 3){
      std::complex<float> data = LFM(ii % signal_length) * SteerVector(2);
      ((int16_t*)(p_dat))[4]   = data.real() * _2_14_; /* Real (I) */
      ((int16_t*)(p_dat))[5]   = data.imag() * _2_14_; /* Imag (Q) */  
    }if (check_sum >= 4){
      std::complex<float> data = LFM(ii % signal_length) * SteerVector(3);
      ((int16_t*)(p_dat))[6]   = data.real() * _2_14_; /* Real (I) */
      ((int16_t*)(p_dat))[7]   = data.imag() * _2_14_; /* Imag (Q) */    
    }  
    ii += 1;
  }
    
	// Schedule TX buffer
	nbytes_tx = iio_buffer_push(txbuf);
	if (nbytes_tx < 0) { printf("Error pushing buf %d\n", (int) nbytes_tx); shutdown(); }
  
  int index = 0;
  Eigen::ArrayXXcf data = Eigen::ArrayXXcf::Zero(check_sum, pulse_number);
  std::complex<int16_t *> ** data16 = new std::complex<int16_t *> * [check_sum];
  for (int ii = 0; ii < check_sum; ii++){
    data16[ii] = new std::complex<int16_t *> [pulse_number];
  }
  
  printf("* Starting IO streaming (press CTRL+C to cancel)\n");
	while (!stop)
	{
		// Refill RX buffer
		nbytes_rx = iio_buffer_refill(rxbuf);
		if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int) nbytes_rx); shutdown(); }
    // memcpy(&buffer[ii * pulse_number * 2], iio_buffer_start(rxbuf), pulse_number * 2 * sizeof(int16_t));
    if (index < 5) { index++; continue; } // 只需要后面的数据
    else { }
    
    auto start = std::chrono::high_resolution_clock::now();
    // GetData_MultiThread(data, rxbuf, 8); // 300000us
    // GetData(data, rxbuf); // Single Thread
    GetData16(data16, check_sum, pulse_number, rxbuf); // 18000us
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    //int ChannelStart = PulseDetection(data, conf.PulseNum, conf.PulseLength, 2, conf.SignalLength);
    //Eigen::ArrayXXcf open_ = data.block(0, ChannelStart, check_sum, conf.PulseNum * signal_length);
    std::cout << duration.count() << std::endl;
    
	}
 
  for (int ii = 0; ii < check_sum; ii++){
    delete [] data16[ii];
  }
  delete [] data16;
 
	shutdown();
	return 0;
}
