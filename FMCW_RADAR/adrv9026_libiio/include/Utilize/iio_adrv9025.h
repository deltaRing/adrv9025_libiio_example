#ifndef _IIO_ADRV9025_H_
#define _IIO_ADRV9025_H_

#include "../../include/Utilize/common_define.h"


// get the device channels from axi-9025-rx-hpc or axi-9025-tx-hpc
const char * get_channel_name(const char* type, int id, char modify);

/* returns adrv9025 phy device */
struct iio_device* get_adrv9025_phy(void);

/* finds adrv9025 streaming IIO devices */
bool get_adrv9025_stream_dev(enum iodev d, struct iio_device **dev);

/* finds adrv9025 streaming IIO channels */
// d is RX or TX, dev is rx/tx define, chid is the channel id, modify is i or q, chn
bool get_adrv9025_stream_ch(enum iodev d, struct iio_device *dev, int chid, char modify, struct iio_channel **chn);

/* finds adrv9025 phy IIO configuration channel with id chid */
bool get_phy_chan(enum iodev d, int chid, struct iio_channel **chn);

/* finds adrv9025 local oscillator IIO configuration channels */
bool get_lo_chan1(struct iio_channel **chn);
bool get_lo_chan2(struct iio_channel **chn);

/* applies streaming configuration through IIO */
bool cfg_adrv9025_streaming_ch(struct stream_cfg *cfg, int chid);

bool cfg_adrv9025_streaming_ch_LO(struct stream_cfg *cfg);
bool cfg_adrv9025_streaming_ch_rx(int chid);
bool cfg_adrv9025_streaming_ch_tx(int chid);

#endif