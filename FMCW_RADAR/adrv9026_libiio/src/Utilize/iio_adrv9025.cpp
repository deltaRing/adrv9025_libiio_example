#include "../../include/Utilize/iio_adrv9025.h"

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

bool cfg_adrv9025_streaming_ch_LO(struct stream_cfg *cfg){
  struct iio_channel *chn = NULL;
  // Configure LO channel
	printf("* Acquiring ADRV9025 TRX lo channel\n");
	if (!get_lo_chan1(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
	if (!get_lo_chan2(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
  return true;
}

bool cfg_adrv9025_streaming_ch_rx(int chid){
  struct iio_channel *chn = NULL;
  
  printf("* Acquiring ADRV9025 phy RX channel %d\n", chid);
	if (!get_phy_chan(RX, chid, &chn)) {	return false; }
  wr_ch_str(chn, "gain_control_mode", "hybrid");
  wr_ch_lli(chn, "sampling_frequency", 245760000); 
  return true;
}

bool cfg_adrv9025_streaming_ch_tx(int chid){
  struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring ADRV9025 phy TX channel %d\n", chid);
	if (!get_phy_chan(TX, chid, &chn)) {	return false; }
  wr_ch_d(chn, "hardwaregain", 0.0); 
  wr_ch_b(chn, "quadrature_tracking_en", true); 
  wr_ch_b(chn, "lo_leakage_tracking_en", true); 
  wr_ch_lli(chn, "sampling_frequency", 245760000);
  return true;
}

/* applies streaming configuration through IIO */
bool cfg_adrv9025_streaming_ch(struct stream_cfg *cfg, int chid)
{
	struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring ADRV9025 phy TX channel %d\n", chid);
	if (!get_phy_chan(TX, chid, &chn)) {	return false; }
  wr_ch_d(chn, "hardwaregain", 0.0); 
  wr_ch_b(chn, "quadrature_tracking_en", true); 
  wr_ch_b(chn, "lo_leakage_tracking_en", true); 
  wr_ch_lli(chn, "sampling_frequency", 245760000);
  
  printf("* Acquiring ADRV9025 phy RX channel %d\n", chid);
	if (!get_phy_chan(RX, chid, &chn)) {	return false; }
  wr_ch_str(chn, "gain_control_mode", "hybrid");
  wr_ch_lli(chn, "sampling_frequency", 245760000); 
  
	// Configure LO channel
	printf("* Acquiring ADRV9025 TRX lo channel\n");
	if (!get_lo_chan1(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
	if (!get_lo_chan2(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
  // altvoltage0: LO1 (output)
  // attr  0: frequency (out_altvoltage0_LO1_frequency) value: 2500000000
	return true;
}