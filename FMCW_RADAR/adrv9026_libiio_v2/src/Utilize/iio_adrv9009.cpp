#include "../../include/Utilize/iio_adrv9009.h"

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
extern struct iio_buffer  *rxbuf;
extern struct iio_buffer  *txbuf;

/* returns adrv9009 phy device */
struct iio_device* get_adrv9009_phy(void)
{
	struct iio_device *dev =  iio_context_find_device(ctx, "adrv9009-phy");
	IIO_ENSURE(dev && "No adrv9009-phy found");
	return dev;
}

/* finds adrv9009 streaming IIO devices */
bool get_adrv9009_stream_dev(enum iodev d, struct iio_device **dev)
{
	switch (d) {
	case TX: {
    *dev = iio_context_find_device(ctx, "axi-adrv9009-tx-hpc"); 
    printf("TX axi-adrv9009-tx-hpc\n");
    return *dev != NULL;
  }
	case RX: { 
    *dev = iio_context_find_device(ctx, "axi-adrv9009-rx-hpc"); 
    printf("RX axi-adrv9009-rx-hpc\n");
    return *dev != NULL;
  }
	default: IIO_ENSURE(0); return false;
	}
}

//
const char * get_ch_name_TX_9009(const char * chan, int id){
  snprintf(tmpstr, sizeof(tmpstr), "%s%d", chan, id);
  return tmpstr;
}

const char * get_ch_name_RX_9009(const char * chan, int id, char iq){
  snprintf(tmpstr, sizeof(tmpstr), "%s%d_%c", chan, id, iq);
  return tmpstr;
}

/* finds adrv9009 streaming IIO channels */
// d is RX or TX, dev is rx/tx define, chid is the channel id, modify is i or q, chn
bool get_adrv9009_stream_ch(enum iodev d, struct iio_device *dev, int chid, char modify, struct iio_channel **chn)
{
  if (d == TX) {
    *chn = iio_device_find_channel(dev, get_ch_name_TX_9009("voltage", chid), d == TX);
  	if (!*chn){
      *chn = iio_device_find_channel(dev, get_ch_name_TX_9009("voltage", chid), d == TX);
    }   
  }else {
    *chn = iio_device_find_channel(dev, get_ch_name_RX_9009("voltage", chid, modify), d == TX);
  	if (!*chn){
      *chn = iio_device_find_channel(dev, get_ch_name_RX_9009("voltage", chid, modify), d == TX);
    }   
  }
  return *chn != NULL;
}

/* finds adrv9009 phy IIO configuration channel with id chid */
bool get_phy_chan_9009(enum iodev d, int chid, struct iio_channel **chn)
{
	switch (d) {
	case RX: {
    *chn = iio_device_find_channel(get_adrv9009_phy(), get_ch_name("voltage", chid), false);
    return *chn != NULL;
  }
	case TX: {
    *chn = iio_device_find_channel(get_adrv9009_phy(), get_ch_name("voltage", chid), true);
    return *chn != NULL;
  }
	default: IIO_ENSURE(0); return false;
	}
}

/* finds adrv9009 local oscillator IIO configuration channels */
bool get_lo_chan(struct iio_channel **chn)
{
  printf("%s\n", get_ch_name("altvoltage", 0));
	 // LO chan is always output, i.e. true
	*chn = iio_device_find_channel(get_adrv9009_phy(), get_ch_name("altvoltage", 0), true);
  return *chn != NULL;
}

bool cfg_adrv9009_streaming_ch_LO(struct stream_cfg *cfg){
  struct iio_channel *chn = NULL;
  // Configure LO channel
	printf("* Acquiring ADRV9009 TRX lo channel\n");
	if (!get_lo_chan(&chn)) { return false; } wr_ch_lli(chn, "frequency", cfg->lo_hz);
  return true;
}

bool cfg_adrv9009_streaming_ch_rx(int chid){
  struct iio_channel *chn = NULL;
  
  printf("* Acquiring ADRV9009 phy RX channel %d\n", chid);
	if (!get_phy_chan_9009(RX, chid, &chn)) {	return false; }
  // wr_ch_str(chn, "gain_control_mode", "hybrid");
  wr_ch_lli(chn, "sampling_frequency", 245760000); 
  return true;
}

bool cfg_adrv9009_streaming_ch_tx(int chid){
  struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring ADRV9009 phy TX channel %d\n", chid);
	if (!get_phy_chan_9009(TX, chid, &chn)) {	return false; }
  wr_ch_d(chn, "hardwaregain", 0.0); 
  // wr_ch_b(chn, "quadrature_tracking_en", true); 
  // wr_ch_b(chn, "lo_leakage_tracking_en", true); 
  wr_ch_lli(chn, "sampling_frequency", 245760000);
  return true;
}