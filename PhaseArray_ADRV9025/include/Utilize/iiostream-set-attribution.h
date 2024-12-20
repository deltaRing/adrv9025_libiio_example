#ifndef _IIOSTREAM_SET_ATTRIBUTION_H_
#define _IIOSTREAM_SET_ATTRIBUTION_H_

#include "test_signal.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

#include <iio.h>

/* helper macros */
#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

#define IIO_ENSURE(expr) { \
	if (!(expr)) { \
		(void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
		(void) abort(); \
	} \
}

/* RX is input, TX is output */
enum iodev { RX, TX };

/* common RX and TX streaming params */
struct stream_cfg {
	long long lo_hz; // Local oscillator frequency in Hz
	long long fs_hz; // sample rate in Hz
	long long bw_hz; // analog bandwidth in Hz
};

/* cleanup and exit */
void shutdown(void);

void handle_sig(int sig);

/* check return value of attr_write function */
void errchk(int v, const char* what);

/* write attribute: long long int */
void wr_ch_lli(struct iio_channel *chn, const char* what, long long val);

void wr_ch_d(struct iio_channel *chn, const char* what, double val);

/* write attribute: long long int */
long long rd_ch_lli(struct iio_channel *chn, const char* what);

#if 0
/* write attribute: string */
void wr_ch_str(struct iio_channel *chn, const char* what, const char* str);
#endif

/* helper function generating channel names */
char* get_ch_name_mod(const char* type, int id, char modify);

/* helper function generating channel names */
char* get_ch_name(const char* type, int id);

/* returns adrv9025 phy device */
struct iio_device* get_adrv9025_phy(void);

/* finds adrv9025 streaming IIO devices */
bool get_adrv9025_stream_dev(enum iodev d, struct iio_device **dev);

// get the device channels from axi-9025-rx-hpc or axi-9025-tx-hpc
const char * get_channel_name(const char* type, int id, char modify);

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


#endif