// Phased Array Radar Functions
#include "include/Algorithms/LFM.h"
#include "include/Algorithms/Beamforming.h"
#include "include/Algorithms/PulseDetection.h"
// Load Waveforms and Set up Adrv9025
#include "include/Utilize/data_convert.h"
#include "include/Utilize/iio_adrv9025.h"
#include "include/Utilize/iio_adrv9009.h"
// FMCW-MIMO Radar Algorithms
#include "include/FMCW_MIMO/FMCW_LFM.h"
#include "include/FMCW_MIMO/FMCW_MIMO_Algorithm.h"
// Standard Functions
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
// Commonly Used define
#include "include/Utilize/common_define.h"
#include "include/Settings/UsingDifferentModule.h"
// DMA Define
#include "include/DMA_PROXY/dma_functions.h"
#include "include/DMA_PROXY/process_LFM.h"

extern Eigen::ArrayXXcf CalculatedRx[_Calculated_Pulse_Number_];
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
extern bool stop;
extern int FFTN;
extern int vFTN;

extern int check_RX, check_TX;
int check_acTx = 0; // actual emit
extern int signal_length; // mLFM.cols()
extern int pulse_number; // all pulse number

//
extern Eigen::ArrayXcf FMCW_waveform;

// LFM define
MIMO_LFM_conf conf;

// DMA define
std::vector<struct channel *> chan_tx, chan_rx;

// 
void Send_Recv(iio_buffer * txbuf, int16_t * _data_tx_, 
              iio_buffer * rxbuf, int16_t * _data_rx_, bool & new_data) {
  ssize_t nbytes_tx, nbytes_rx;
  int index = 0, count = 0;
  
  Eigen::ArrayXXcf data(check_RX, pulse_number);
  Eigen::ArrayXXcf PulseCompressed(_Calculated_Pulse_Number_, 8192); // FFTN
  Eigen::ArrayXXcf MotionTargetDetection(64, 8192);
                
  while (!stop)
	{
#if (_USE_CYLIC_BUFFER_ == 0)
    if (new_data) {
       for (int ii = 0; ii < _DMA_Number_; ii++){
          long length = (long)iio_buffer_end(txbuf) - (long)iio_buffer_start(txbuf);
          memcpy((void*)iio_buffer_start(txbuf), (void*)_data_tx_, length);
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
#else
    // Cylic buffer shall not re-push
    if (new_data) {
       for (int ii = 0; ii < _DMA_Number_; ii++){
          long length = (long)iio_buffer_end(txbuf) - (long)iio_buffer_start(txbuf);
          memcpy((void*)iio_buffer_start(txbuf), (void*)_data_tx_, length);
          // Schedule TX buffer
          nbytes_tx = iio_buffer_push(txbuf);
       }
       new_data = false;
       continue;
    }
#endif
    // Refill RX buffer
    nbytes_rx = iio_buffer_refill(rxbuf);
    long length = (long)iio_buffer_end(rxbuf) - (long)iio_buffer_start(rxbuf);
    
    if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int)nbytes_rx); shutdown(); }
    else { }
    if (index < 100) { index++; usleep(10); continue; }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    GetData_MultiThread(data, rxbuf, 4);
    // GetRawSignal(data, check_RX, rxbuf, chan_tx[2], chan_rx[2]);
    
    // Reduce Time
    int startindex = PulseDetection_LowPulse(data, check_acTx, signal_length / check_acTx);
    
    // by default we only process
    
    GetFFTData(PulseCompressed, data, conf.FMCW_wave_conj, chan_tx, 
                      chan_rx, 8192, _Calculated_Pulse_Number_, signal_length, startindex);
    // Get MTD   
    //GetFFTData(MotionTargetDetection, PulseCompressed, conf.FMCW_wave_conj, chan_tx, 
    //                  chan_rx, 64, _Calculated_Pulse_Number_, signal_length, startindex);
    
    auto end = std::chrono::high_resolution_clock::now(); 
    
    std::chrono::duration<double> elapsed = end - start; // too slow!
    
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;
    
    std::ofstream rraw("rraw.txt"), iraw("iraw.txt");

    rraw << PulseCompressed.real();
    iraw << PulseCompressed.imag();
    rraw.close();
    iraw.close();
    dma_deinitial(chan_tx, chan_rx);
    exit(0);
  }
}

/* simple configuration and streaming */
int main (__notused int argc, __notused char **argv)
{
  // Check Channel Number
  check_TX = _ENABLE_CH1_TX_ + _ENABLE_CH2_TX_ + 
             _ENABLE_CH3_TX_ + _ENABLE_CH4_TX_;
  check_RX = _ENABLE_CH1_RX_ + _ENABLE_CH2_RX_ +
             _ENABLE_CH3_RX_ + _ENABLE_CH4_RX_;
  
  // MIMO Config
  MIMO_LFM_configure(conf);
  bool ec[4] = { 1, 0, 0, 0 };
  check_acTx = 1;
  
  Eigen::ArrayXXcf mLFM = MIMO_LFM(check_TX, ec, conf);
  FMCW_waveform = conf.FMCW_wave;
  // LFM signal create
  Eigen::ArrayXXcf mLFMs(check_TX, mLFM.cols() * _Expected_TX_Pulse_Number_);
  for (int ii = 0; ii < _Expected_TX_Pulse_Number_; ii++) {
    mLFMs.block(0, ii * mLFM.cols(), check_TX, mLFM.cols()) = mLFM;
  }
  
  signal_length = mLFMs.cols();
  pulse_number  = (_Calculated_Pulse_Number_ + 2) * mLFM.cols();

	// Stream configuration
	struct stream_cfg trxcfg;
	int err;

	// Listen to ctrl+c and IIO_ENSURE
	signal(SIGINT, handle_sig);

	// TRX stream config
	trxcfg.lo_hz = _LO_;

	Initialize_ADRV9025(signal_length, pulse_number, trxcfg);
 
	char *p_dat, *p_end;
	ssize_t nbytes_rx, nbytes_tx;
	ptrdiff_t p_inc;
  int ii = 0;
  
  printf("Single Pulse Number:%d Total Single Length:%d \n", signal_length, pulse_number);
  
  /* WRITE: Get pointers to TX buf and write IQ to TX buf port 0 */
  int16_t * _data_tx_ = new int16_t[signal_length * 2 * check_TX];
  int16_t * _data_rx_ = new int16_t[pulse_number * 2 * check_RX];
  for (int ii = 0; ii < signal_length; ii++){
    for (int jj = 0; jj < check_TX; jj++){
      _data_tx_[ii * check_TX * 2 + jj * 2]     = mLFMs(jj, ii).real() * _2_13_;
      _data_tx_[ii * check_TX * 2 + jj * 2 + 1] = mLFMs(jj, ii).imag() * _2_13_;
    }
  }
  
  printf("Initializing DMA...\n");
  
  // DMA Initial
  dma_initial(chan_tx, chan_rx);
  
  if (_USE_CYLIC_BUFFER_){ printf("Use The Cylic Buffer\n"); }
  else { printf("Use non-Cylic Buffer\n"); }
  
  printf("* Starting IO streaming (press CTRL+C to cancel)\n");
  bool _new_data_ = true;
  std::thread _thread_ = std::thread(Send_Recv, txbuf, _data_tx_,
                       rxbuf, _data_rx_, std::ref(_new_data_));
  _thread_.detach();
	while (!stop)
	{
     usleep(10);
	}
 
  _thread_.join();
  delete [] _data_tx_;
  delete [] _data_rx_;
  // DMA DeInitial
  dma_deinitial(chan_tx, chan_rx);
	shutdown();
	return 0;
}
