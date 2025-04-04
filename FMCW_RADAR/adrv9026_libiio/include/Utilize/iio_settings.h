#ifndef _IIO_SETTINGS_H_
#define _IIO_SETTINGS_H_

/*
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

*/

#endif