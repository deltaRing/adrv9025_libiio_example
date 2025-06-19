#ifndef _IIO_ADRV9009_H_
#define _IIO_ADRV9009_H_

#include "../../include/Utilize/common_define.h"

/* returns adrv9009 phy device */
struct iio_device* get_adrv9009_phy(void);

/* finds adrv9009 streaming IIO devices */
bool get_adrv9009_stream_dev(enum iodev d, struct iio_device **dev);

const char * get_ch_name_TX_9009(const char * chan, int id);
const char * get_ch_name_RX_9009(const char * chan, int id, char iq);
//
const char * get_ch_name_9009(const char * chan, int id);

/* finds adrv9009 streaming IIO channels */
// d is RX or TX, dev is rx/tx define, chid is the channel id, modify is i or q, chn
bool get_adrv9009_stream_ch(enum iodev d, struct iio_device *dev, int chid, char modify, struct iio_channel **chn);

/* finds adrv9009 phy IIO configuration channel with id chid */
bool get_phy_chan_9009(enum iodev d, int chid, struct iio_channel **chn);

/* finds adrv9009 local oscillator IIO configuration channels */
bool get_lo_chan(struct iio_channel **chn);

bool cfg_adrv9009_streaming_ch_LO(struct stream_cfg *cfg);
bool cfg_adrv9009_streaming_ch_rx(int chid);
bool cfg_adrv9009_streaming_ch_tx(int chid);

#endif