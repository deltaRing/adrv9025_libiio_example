#include "../../include/Utilize/common_define.h"

#include <iio.h>
#include <unistd.h>
#include "../../include/FMCW_MIMO/FMCW_LFM.h"
#include "../../include/Utilize/data_convert.h"
#include "../../include/Algorithms/PulseDetection.h"
#include "../../include/FMCW_MIMO/FMCW_MIMO_Algorithm.h"

/* static scratch mem for strings */
char tmpstr[64];

/* IIO structs required for streaming */
struct iio_context *ctx    = NULL;
// Streaming devices and channels
struct iio_device *tx;
struct iio_device *rx;
struct iio_device *phy;
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
// stop signal
bool stop = false;
int FFTN  = 8192;
int vFTN  = 64;

int check_RX = -1,
    check_TX = -1;
int signal_length = -1; // mLFM.cols() // 发射信号实际长度
int pulse_number  = -1;

void Send_TX(iio_buffer * txbuf, int16_t * _data_, bool & new_data) {
  ssize_t nbytes_tx;
  while (!stop)
	{
     if (new_data) {
       for (int ii = 0; ii < _DMA_Number_; ii++){
          long length = (long)iio_buffer_end(txbuf) - (long)iio_buffer_start(txbuf);
          memcpy((void*)iio_buffer_start(txbuf), (void*)_data_, length);
          // Schedule TX buffer
          nbytes_tx = iio_buffer_push(txbuf);
       }
       new_data = false;
       continue;
     }
    
    // Schedule TX buffer
    nbytes_tx = iio_buffer_push(txbuf);
    if (nbytes_tx < 0) { printf("Error pushing buf %d\n", (int)nbytes_tx); shutdown(); }
    else { }
  }
}

void Recv_RX(iio_buffer * rxbuf, int16_t * _data_rx_, bool & new_data) {
  ssize_t nbytes_rx;
  int index = 0;
  while (!stop)
	{
    // Refill RX buffer
    nbytes_rx = iio_buffer_refill(rxbuf);
    long length = (long)iio_buffer_end(rxbuf) - (long)iio_buffer_start(rxbuf);
    // memcpy((void*)iio_buffer_start(rxbuf), (void*)_data_rx_, length);
    if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int)nbytes_rx); shutdown(); }
    else { }
    //GetData(data, rxbuf); // Single Thread
    
    if (index < 10) { index++; continue; }
    else { }
    if (new_data) {
      index = 0;
      continue; // 这个数据不能使用
    }
  }
}

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
  errchk(iio_channel_attr_write_double(chn, what, val), what);
}

void wr_ch_b(struct iio_channel *chn, const char* what, bool val) {
  errchk(iio_channel_attr_write_bool(chn, what, val), what);
}

/* write attribute: long long int */
long long rd_ch_lli(struct iio_channel *chn, const char* what)
{
	long long val = 0;

	errchk(iio_channel_attr_read_longlong(chn, what, &val), what);

	printf("\t %s: %lld\n", what, val);
	return val;
}

/* write attribute: string */
void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	errchk(iio_channel_attr_write(chn, what, str), what);
}

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

Eigen::MatrixXcf GetDFTMatrix(int points) {
  Eigen::MatrixXcf DFT(points, points);
  
  for (int ii = 0; ii < points; ii++) {
    for (int jj = 0; jj < points; jj++) {
      DFT(ii, jj) = std::complex<float> {
        cos(-2 * 3.1415926535 * ii * jj / points) / sqrt(points),
        sin(-2 * 3.1415926535 * ii * jj / points) / sqrt(points)
      };
    }
  }

  return DFT;
}

