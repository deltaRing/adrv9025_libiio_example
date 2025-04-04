#include "../../include/Settings/UsingDifferentModule.h"

// Sun is shining
// comprising
extern char tmpstr[64];
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

#if _USE_ADRV9025_ == 1

void Initialize_ADRV9025(int signal_length, int pulse_number, struct stream_cfg trxcfg) {

	printf("* Acquiring IIO context\n");
	IIO_ENSURE((ctx = iio_create_default_context()) && "No context");
	IIO_ENSURE(iio_context_get_devices_count(ctx) > 0 && "No devices");

	printf("* Acquiring ADRV9025 streaming devices\n");
	IIO_ENSURE(get_adrv9025_stream_dev(RX, &rx) && "No rx dev found");
	IIO_ENSURE(get_adrv9025_stream_dev(TX, &tx) && "No tx dev found");
 
  printf("* Set ADRV9025 LO to %f\n", trxcfg.lo_hz);
  cfg_adrv9025_streaming_ch_LO(&trxcfg);
   
  printf("* Configuring ADRV9025 TX for streaming\n");
  if (_ENABLE_CH1_TX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_tx(0) && "TX 0 device not found");
 	if (_ENABLE_CH2_TX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_tx(1) && "TX 1 device not found");
 	if (_ENABLE_CH3_TX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_tx(2) && "TX 2 device not found");
 	if (_ENABLE_CH4_TX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_tx(3) && "TX 3 device not found");
  
  printf("* Configuring ADRV9025 RX for streaming\n");
  if (_ENABLE_CH1_RX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_rx(0) && "RX 0 device not found");
 	if (_ENABLE_CH2_RX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_rx(1) && "RX 1 device not found");
 	if (_ENABLE_CH3_RX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_rx(2) && "RX 2 device not found");
 	if (_ENABLE_CH4_RX_) IIO_ENSURE(cfg_adrv9025_streaming_ch_rx(3) && "RX 3 device not found");
  
  printf("* Initializing ADRV9025 Channels IIO streaming channels\n"); 
  if (_ENABLE_CH1_RX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 0, 'i', &rx0_i) && "RX chan 1 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 0, 'q', &rx0_q) && "RX chan 1 q not found");
  }
  if (_ENABLE_CH1_TX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 0, 'i', &tx0_i) && "TX chan 1 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 0, 'q', &tx0_q) && "TX chan 1 q not found");
  }
 
  if (_ENABLE_CH2_RX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 1, 'i', &rx1_i) && "RX chan 2 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 1, 'q', &rx1_q) && "RX chan 2 q not found");
  }
  if (_ENABLE_CH2_TX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 1, 'i', &tx1_i) && "TX chan 2 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 1, 'q', &tx1_q) && "TX chan 2 q not found");
  }
  
  if (_ENABLE_CH3_RX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 2, 'i', &rx2_i) && "RX chan 3 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 2, 'q', &rx2_q) && "RX chan 3 q not found");
  }
  if (_ENABLE_CH3_TX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 2, 'i', &tx2_i) && "TX chan 3 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 2, 'q', &tx2_q) && "TX chan 3 q not found");
  }
  
  if (_ENABLE_CH4_RX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 3, 'i', &rx3_i) && "RX chan 4 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(RX, rx, 3, 'q', &rx3_q) && "RX chan 4 q not found");
  }
  if (_ENABLE_CH4_TX_) {
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 3, 'i', &tx3_i) && "TX chan 4 i not found");
	  IIO_ENSURE(get_adrv9025_stream_ch(TX, tx, 3, 'q', &tx3_q) && "TX chan 4 q not found");
  }
 
  printf("* Enabling IIO streaming channels\n");
  if (_ENABLE_CH1_RX_) {
    printf("* Enabling Channel 1 RX\n");
	  iio_channel_enable(rx0_i);
	  iio_channel_enable(rx0_q);
  }
  if (_ENABLE_CH1_TX_) {
    printf("* Enabling Channel 1 TX\n");
	  iio_channel_enable(tx0_i);
	  iio_channel_enable(tx0_q);
  }
  
  if (_ENABLE_CH2_RX_) {
    printf("* Enabling Channel 2 RX\n");
	  iio_channel_enable(rx1_i);
	  iio_channel_enable(rx1_q);
  }
  if (_ENABLE_CH2_TX_) {
    printf("* Enabling Channel 2 TX\n");
	  iio_channel_enable(tx1_i);
	  iio_channel_enable(tx1_q);
  }
  
  if (_ENABLE_CH3_RX_) {
    printf("* Enabling Channel 3 RX\n");
	  iio_channel_enable(rx2_i);
	  iio_channel_enable(rx2_q);
  }
  if (_ENABLE_CH3_TX_) {
    printf("* Enabling Channel 3 TX\n");
	  iio_channel_enable(tx2_i);
	  iio_channel_enable(tx2_q);
  }
  
  if (_ENABLE_CH4_RX_) {
    printf("* Enabling Channel 4 RX\n");
	  iio_channel_enable(rx3_i);
	  iio_channel_enable(rx3_q);
  }
  if (_ENABLE_CH4_TX_) {
    printf("* Enabling Channel 4 TX\n");
	  iio_channel_enable(tx3_i);
	  iio_channel_enable(tx3_q);
  }
  
	printf("* Creating non-cyclic RX IIO buffers\n");
	rxbuf = iio_device_create_buffer(rx, pulse_number, false);
  if (!rxbuf) {
		perror("Could not create RX buffer");
		shutdown();
 	}
  
  
#if (_USE_CYLIC_BUFFER_ == 1)
	printf("* Creating cyclic TX IIO buffers\n");
  txbuf = iio_device_create_buffer(tx, signal_length, true);
#else 
  printf("* Creating non-cyclic TX IIO buffers\n");
  txbuf = iio_device_create_buffer(tx, _Calculated_Pulse_Number_ * signal_length, false);
#endif  
 	if (!txbuf) {
		perror("Could not create TX buffer");
		shutdown();
 	}


}

#elif _USE_ADRV9009_ == 1

void Initialize_ADRV9009(int signal_length, int pulse_number, struct stream_cfg trxcfg) {
  printf("* Acquiring IIO context\n");
	IIO_ENSURE((ctx = iio_create_default_context()) && "No context");
	IIO_ENSURE(iio_context_get_devices_count(ctx) > 0 && "No devices");

	printf("* Acquiring ADRV9009 streaming devices\n");
	IIO_ENSURE(get_adrv9009_stream_dev(RX, &rx) && "No rx dev found");
	IIO_ENSURE(get_adrv9009_stream_dev(TX, &tx) && "No tx dev found");
 
  printf("* Set ADRV9009 LO to %f\n", trxcfg.lo_hz);
  cfg_adrv9009_streaming_ch_LO(&trxcfg);
   
  printf("* Configuring ADRV9009 TX for streaming\n");
  if (_ENABLE_CH1_TX_) IIO_ENSURE(cfg_adrv9009_streaming_ch_rx(0) && "TX 0 device not found");
 	if (_ENABLE_CH2_TX_) IIO_ENSURE(cfg_adrv9009_streaming_ch_rx(1) && "TX 1 device not found");
  
  printf("* Configuring ADRV9009 RX for streaming\n");
  if (_ENABLE_CH1_RX_) IIO_ENSURE(cfg_adrv9009_streaming_ch_rx(0) && "RX 0 device not found");
 	if (_ENABLE_CH2_RX_) IIO_ENSURE(cfg_adrv9009_streaming_ch_rx(1) && "RX 1 device not found");
  
  printf("* Initializing ADRV9009 Channels IIO streaming channels\n"); 
  if (_ENABLE_CH1_RX_) {
	  IIO_ENSURE(get_adrv9009_stream_ch(RX, rx, 0, 'i', &rx0_i) && "RX chan 1 i not found");
	  IIO_ENSURE(get_adrv9009_stream_ch(RX, rx, 0, 'q', &rx0_q) && "RX chan 1 q not found");
  }
  if (_ENABLE_CH1_TX_) {
	  IIO_ENSURE(get_adrv9009_stream_ch(TX, tx, 0, 'x', &tx0_i) && "TX chan 1 i not found");
	  IIO_ENSURE(get_adrv9009_stream_ch(TX, tx, 1, 'x', &tx0_q) && "TX chan 1 q not found");
  }
 
  if (_ENABLE_CH2_RX_) {
	  IIO_ENSURE(get_adrv9009_stream_ch(RX, rx, 1, 'i', &rx1_i) && "RX chan 2 i not found");
	  IIO_ENSURE(get_adrv9009_stream_ch(RX, rx, 1, 'q', &rx1_q) && "RX chan 2 q not found");
  }
  if (_ENABLE_CH2_TX_) {
	  IIO_ENSURE(get_adrv9009_stream_ch(TX, tx, 2, 'x', &tx1_i) && "TX chan 2 i not found");
	  IIO_ENSURE(get_adrv9009_stream_ch(TX, tx, 3, 'x', &tx1_q) && "TX chan 2 q not found");
  }
 
  printf("* Enabling IIO streaming channels\n");
  if (_ENABLE_CH1_RX_) {
    printf("* Enabling Channel 1 RX\n");
	  iio_channel_enable(rx0_i);
	  iio_channel_enable(rx0_q);
  }
  if (_ENABLE_CH1_TX_) {
    printf("* Enabling Channel 1 TX\n");
	  iio_channel_enable(tx0_i);
	  iio_channel_enable(tx0_q);
  }
  
  if (_ENABLE_CH2_RX_) {
    printf("* Enabling Channel 2 RX\n");
	  iio_channel_enable(rx1_i);
	  iio_channel_enable(rx1_q);
  }
  if (_ENABLE_CH2_TX_) {
    printf("* Enabling Channel 2 TX\n");
	  iio_channel_enable(tx1_i);
	  iio_channel_enable(tx1_q);
  }
  
	printf("* Creating non-cyclic RX IIO buffers\n");
	rxbuf = iio_device_create_buffer(rx, pulse_number, false);
  if (!rxbuf) {
		perror("Could not create RX buffer");
		shutdown();
 	}
  
	printf("* Creating non-cyclic TX IIO buffers\n");
  txbuf = iio_device_create_buffer(tx, signal_length, false);
 	if (!txbuf) {
		perror("Could not create TX buffer");
		shutdown();
 	}
}


#else

#endif