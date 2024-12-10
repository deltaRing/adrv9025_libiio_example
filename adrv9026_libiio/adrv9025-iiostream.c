// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * libiio - ADRV9025 IIO streaming example
 *
 * Copyright (C) 2014 IABG mbH
 * Author: Michael Feilen <feilen_at_iabg.de>
 * Copyright (C) 2019 Analog Devices Inc.
 **/

#include "iiostream-set-attribution.h"

/* static scratch mem for strings */
extern char tmpstr[64];

// the buffer
int16_t buffer_i[BLOCK_SIZE];
int16_t buffer_q[BLOCK_SIZE];

extern bool stop;

/* IIO structs required for streaming */
extern struct iio_context *ctx;
extern struct iio_device *tx;
extern struct iio_device *rx;
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

/* simple configuration and streaming */
int main (__notused int argc, __notused char **argv)
{
  // Initialize Waveform
  float tc = 1.6667e-4 * 2, B=4.7e6, fs = 245760000; // 10 MHz Bandwidth
  cosine_signal(buffer_i, buffer_q, B, fs);  
  // LFM_signal(buffer_i, buffer_q, B, tc, fs);

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
	trxcfg.lo_hz = GHZ(1.8);

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
	rxbuf = iio_device_create_buffer(rx, BLOCK_SIZE, false);
  if (!rxbuf) {
		perror("Could not create RX buffer");
		shutdown();
 	}
	printf("* Creating cyclic TX IIO buffers\n");
  txbuf = iio_device_create_buffer(tx, BLOCK_SIZE, true);
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
  p_end = iio_buffer_end(txbuf);
  uint8_t check_sum = _ENABLE_CH1_ + _ENABLE_CH2_ + _ENABLE_CH3_ + _ENABLE_CH4_;
  for (p_dat = iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
    if (check_sum == 1){
      ((int16_t*)(p_dat))[0] = buffer_i[ii % BLOCK_SIZE]; /* Real (I) */
      ((int16_t*)(p_dat))[1] = buffer_q[ii % BLOCK_SIZE]; /* Imag (Q) */    
    }else if (check_sum == 2){
      ((int16_t*)(p_dat))[2] = buffer_i[ii % BLOCK_SIZE]; /* Real (I) */
      ((int16_t*)(p_dat))[3] = buffer_q[ii % BLOCK_SIZE]; /* Imag (Q) */  
    }else if (check_sum == 3){
      ((int16_t*)(p_dat))[4] = buffer_i[ii % BLOCK_SIZE]; /* Real (I) */
      ((int16_t*)(p_dat))[5] = buffer_q[ii % BLOCK_SIZE]; /* Imag (Q) */  
    }else if (check_sum == 4){
      ((int16_t*)(p_dat))[6] = buffer_i[ii % BLOCK_SIZE]; /* Real (I) */
      ((int16_t*)(p_dat))[7] = buffer_q[ii % BLOCK_SIZE]; /* Imag (Q) */    
    }else {
      printf("* Invalid Channel Settings\n");
      return -1;
    }      
    ii += 1;
  }
    
	nbytes_tx = iio_buffer_push(txbuf);
	if (nbytes_tx < 0) { printf("Error pushing buf %d\n", (int) nbytes_tx); shutdown(); }
  
  printf("* Starting IO streaming (press CTRL+C to cancel)\n");
	while (!stop)
	{
		// Schedule TX buffer
		// nbytes_tx = iio_buffer_push(txbuf);
		// if (nbytes_tx < 0) { printf("Error pushing buf %d\n", (int) nbytes_tx); shutdown(); }
 
		// Refill RX buffer
		nbytes_rx = iio_buffer_refill(rxbuf);
		if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int) nbytes_rx); shutdown(); }
    
    p_inc = iio_buffer_step(rxbuf);
    p_end = iio_buffer_end(rxbuf);
    for (p_dat = iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc) {
      int16_t i1 = -1, q1 = -1, i2 = -1, q2 = -1,
              i3 = -1, q3 = -1, i4 = -1, q4 = -1;
      if (check_sum == 1) {
        i1 = ((int16_t*)p_dat)[0]; /* Real (I) */
        q1 = ((int16_t*)p_dat)[1]; /* Imag (Q) */ 
      }
      else if (check_sum == 2) {
        i2 = ((int16_t*)p_dat)[2]; /* Real (I) */
        q2 = ((int16_t*)p_dat)[3]; /* Imag (Q) */ 
      }
      else if (check_sum == 3) {
        i3 = ((int16_t*)p_dat)[4]; /* Real (I) */
        q3 = ((int16_t*)p_dat)[5]; /* Imag (Q) */
      }
      else if (check_sum == 4) {
        i4 = ((int16_t*)p_dat)[6]; /* Real (I) */
        q4 = ((int16_t*)p_dat)[7]; /* Imag (Q) */ 
      } 
    }
    
    return 0;
	}

	shutdown();

	return 0;
}
