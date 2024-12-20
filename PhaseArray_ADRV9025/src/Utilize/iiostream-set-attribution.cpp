#include "../../include/Utilize/iiostream-set-attribution.h"

/* static scratch mem for strings */
char tmpstr[64];

/* IIO structs required for streaming */
struct iio_context *ctx    = NULL;
// Streaming devices and channels
struct iio_device *tx;
struct iio_device *rx;
struct iio_channel *rx0_i = NULL;
struct iio_channel *rx0_q = NULL;
struct iio_channel *tx0_i = NULL;
struct iio_channel *tx0_q = NULL;
struct iio_channel *rx1_i = NULL;
struct iio_channel *rx1_q = NULL;
struct iio_channel *tx1_i = NULL;
struct iio_channel *tx1_q = NULL;
struct iio_channel *rx2_i = NULL;
struct iio_channel *rx2_q = NULL;
struct iio_channel *tx2_i = NULL;
struct iio_channel *tx2_q = NULL;
struct iio_channel *rx3_i = NULL;
struct iio_channel *rx3_q = NULL;
struct iio_channel *tx3_i = NULL;
struct iio_channel *tx3_q = NULL;
struct iio_buffer  *rxbuf = NULL;
struct iio_buffer  *txbuf = NULL;

bool stop = false;

/* cleanup and exit */
void shutdown(void)
{
	printf("* Destroying buffers\n");
	if (rxbuf) { iio_buffer_destroy(rxbuf); }
	if (txbuf) { iio_buffer_destroy(txbuf); }

	printf("* Disabling streaming channels\n");
	if (rx0_i) { iio_channel_disable(rx0_i); }
	if (rx0_q) { iio_channel_disable(rx0_q); }
	if (tx0_i) { iio_channel_disable(tx0_i); }
	if (tx0_q) { iio_channel_disable(tx0_q); }
	if (rx1_i) { iio_channel_disable(rx1_i); }
	if (rx1_q) { iio_channel_disable(rx1_q); }
	if (tx1_i) { iio_channel_disable(tx1_i); }
	if (tx1_q) { iio_channel_disable(tx1_q); }
	if (rx2_i) { iio_channel_disable(rx2_i); }
	if (rx2_q) { iio_channel_disable(rx2_q); }
	if (tx2_i) { iio_channel_disable(tx2_i); }
	if (tx2_q) { iio_channel_disable(tx2_q); }
	if (rx3_i) { iio_channel_disable(rx3_i); }
	if (rx3_q) { iio_channel_disable(rx3_q); }
	if (tx3_i) { iio_channel_disable(tx3_i); }
	if (tx3_q) { iio_channel_disable(tx3_q); }

	printf("* Destroying context\n");
	if (ctx) { iio_context_destroy(ctx); }
	exit(0);
}

void handle_sig(int sig)
{
	printf("Waiting for process to finish... Got signal %d\n", sig);
	stop = true;
}

/* check return value of attr_write function */
void errchk(int v, const char* what) {
	 if (v < 0) { fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); shutdown(); }
}

/* write attribute: long long int */
void wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	errchk(iio_channel_attr_write_longlong(chn, what, val), what);
}

void wr_ch_d(struct iio_channel *chn, const char* what, double val) {
  errchk(iio_channel_attr_read_double(chn, what, &val), what);
}

/* write attribute: long long int */
long long rd_ch_lli(struct iio_channel *chn, const char* what)
{
	long long val = 0;

	errchk(iio_channel_attr_read_longlong(chn, what, &val), what);

	printf("\t %s: %lld\n", what, val);
	return val;
}

#if 0
/* write attribute: string */
void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	errchk(iio_channel_attr_write(chn, what, str), what);
}
#endif

/* helper function generating channel names */
char* get_ch_name_mod(const char* type, int id, char modify)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d_%c", type, id, modify);
	return tmpstr;
}

/* helper function generating channel names */
char* get_ch_name(const char* type, int id)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
	return tmpstr;
}

/* returns adrv9025 phy device */
struct iio_device* get_adrv9025_phy(void)
{
	struct iio_device *dev =  iio_context_find_device(ctx, "adrv9025-phy");
	IIO_ENSURE(dev && "No adrv9025-phy found");
	return dev;
}

/* finds adrv9025 streaming IIO devices */
bool get_adrv9025_stream_dev(enum iodev d, struct iio_device **dev)
{
	switch (d) {
	case TX: {
    *dev = iio_context_find_device(ctx, "axi-adrv9025-tx-hpc"); 
    printf("TX axi-adrv9025-tx-hpc\n");
    return *dev != NULL;
  }
	case RX: { 
    *dev = iio_context_find_device(ctx, "axi-adrv9025-rx-hpc"); 
    printf("RX axi-adrv9025-rx-hpc\n");
    return *dev != NULL;
  }
	default: IIO_ENSURE(0); return false;
	}
}

// get the device channels from axi-9025-rx-hpc or axi-9025-tx-hpc
const char * get_channel_name(const char* type, int id, char modify){
  snprintf(tmpstr, sizeof(tmpstr), "%s%d_%c", type, id, modify);
  return tmpstr;
}

/* finds adrv9025 streaming IIO channels */
// d is RX or TX, dev is rx/tx define, chid is the channel id, modify is i or q, chn
bool get_adrv9025_stream_ch(enum iodev d, struct iio_device *dev, int chid, char modify, struct iio_channel **chn)
{
  *chn = iio_device_find_channel(dev, get_channel_name("voltage", chid, modify), d == TX);
	if (!*chn){
    *chn = iio_device_find_channel(dev, get_channel_name("voltage", chid, modify), d == TX);
  }   
  return	*chn != NULL;
}

/* finds adrv9025 phy IIO configuration channel with id chid */
bool get_phy_chan(enum iodev d, int chid, struct iio_channel **chn)
{
	switch (d) {
	case RX: {
    *chn = iio_device_find_channel(get_adrv9025_phy(), get_ch_name("voltage", chid), false);
    return *chn != NULL;
  }
	case TX: {
    *chn = iio_device_find_channel(get_adrv9025_phy(), get_ch_name("voltage", chid), true);
    return *chn != NULL;
  }
	default: IIO_ENSURE(0); return false;
	}
}

/* finds adrv9025 local oscillator IIO configuration channels */
bool get_lo_chan1(struct iio_channel **chn)
{
  printf("%s\n", get_ch_name("altvoltage", 0));
	 // LO chan is always output, i.e. true
	*chn = iio_device_find_channel(get_adrv9025_phy(), get_ch_name("altvoltage", 0), true);
  return *chn != NULL;
}

/* finds adrv9025 local oscillator IIO configuration channels */
bool get_lo_chan2(struct iio_channel **chn)
{
  printf("%s\n", get_ch_name("altvoltage", 1));
	 // LO chan is always output, i.e. true
	*chn = iio_device_find_channel(get_adrv9025_phy(), get_ch_name("altvoltage", 1), true);
  return *chn != NULL;
}

/* finds adrv9025 local oscillator IIO configuration channels */
bool get_lo_chan3(struct iio_channel **chn)
{
  printf("%s\n", get_ch_name("altvoltage", 2));
	 // LO chan is always output, i.e. true
	*chn = iio_device_find_channel(get_adrv9025_phy(), get_ch_name("altvoltage", 2), true);
  return *chn != NULL;
}

/* applies streaming configuration through IIO */
bool cfg_adrv9025_streaming_ch(struct stream_cfg *cfg, int chid)
{
	struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring ADRV9025 phy TX channel %d\n", chid);
	if (!get_phy_chan(TX, chid, &chn)) {	return false; }
  wr_ch_d(chn, "hardwaregain", 0.0); 
  
  printf("* Acquiring ADRV9025 phy RX channel %d\n", chid);
	if (!get_phy_chan(RX, chid, &chn)) {	return false; }
  
	// Configure LO channel
	printf("* Acquiring ADRV9025 TRX lo channel\n");
	if (!get_lo_chan1(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
	if (!get_lo_chan2(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
  // altvoltage0: LO1 (output)
  // attr  0: frequency (out_altvoltage0_LO1_frequency) value: 2500000000
	return true;
}