#include "include/LOL/LOL.h"
#include "include/Settings/TestFFTTime.h"
#include "include/Algorithms/RadarFunctions.h"

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
extern bool stop;
extern int FFTN;
extern int vFTN;

extern int check_RX, check_TX;
extern int signal_length; // mLFM.cols()
extern int pulse_number; // all pulse number
int check_acTx = 0; // actual emit

// LFM define
MIMO_LFM_conf conf;

// Pulse LFM define
PulseRadar pLFMparams;

// PROCESS define
process_param params;

// DMA define
std::vector<struct channel *> chan_tx, chan_rx;

// UDP define
struct sockaddr_in serverAddr;
int sockfd;

/* simple configuration and streaming */
int main (__notused int argc, __notused char **argv)
{
  printf("Initializing DMA...\n");
  
  // DMA Initial
  dma_initial(chan_tx, chan_rx);

  // load config file
  std::unordered_map<std::string, std::vector<float>> confData;
  bool load_succ = read_config_file("config.txt", confData);
  bool ec[4] = {1, 0, 0, 0};
  
  // Initialize the UDP
  init_socket(serverAddr, sockfd);

  // Check Channel Number
  check_TX = _ENABLE_CH1_TX_ + _ENABLE_CH2_TX_ + 
             _ENABLE_CH3_TX_ + _ENABLE_CH4_TX_;
  check_RX = _ENABLE_CH1_RX_ + _ENABLE_CH2_RX_ +
             _ENABLE_CH3_RX_ + _ENABLE_CH4_RX_;
  
  // Waveform define
  Eigen::ArrayXXcf mLFM, mLFMs;
  
 	// Stream configuration
 	struct stream_cfg trxcfg;
 	int err;
  int type = 0; // test: 2 lfm: 1 fmcw: 0
  
  // Listen to ctrl+c and IIO_ENSURE
 	signal(SIGINT, handle_sig);
  
  if (load_succ) {
    printf("*** Successfully Load Config File ***\n");
  
    parse_config_file(confData, type, conf, pLFMparams, trxcfg, params, ec);
    for (int ii = 0; ii < 4; ii++) check_acTx += ec[ii];
    
    // mLFM signal initialized
    if (type == 1.0) {
      mLFM = PulseRadarWaveform(pLFMparams, chan_tx, chan_rx);
    } else if (type == 0.0) {
      mLFM = MIMO_LFM(check_TX, ec, conf);
    }else {
      mLFM = LOL_Generate(check_TX);
    }
    
  } else {
    printf("*** Failed Load Config File, Using the Default Parameters ***\n");
    
    // MIMO Config
    MIMO_LFM_configure(conf);
    for (int ii = 0; ii < 4; ii++) check_acTx += ec[ii];
    
    // Initialize LFM Signal
    mLFM = MIMO_LFM(check_TX, ec, conf);
    
  	// TRX stream config
  	trxcfg.lo_hz = _LO_;
  }
  
  // LFM signal create
  mLFMs = Eigen::ArrayXXcf(check_TX, mLFM.cols() * _Expected_TX_Pulse_Number_);
  for (int ii = 0; ii < _Expected_TX_Pulse_Number_; ii++) {
    mLFMs.block(0, ii * mLFM.cols(), check_TX, mLFM.cols()) = mLFM;
  }
    
  int PN = 1;
  // Calculate the actual signal pulse number and initialize the adrv9025
  if (type == 0){
    PN = int(conf.PulseNum);
    signal_length = mLFMs.cols();
    pulse_number  = (PN + 2) * mLFMs.cols();
  }else if (type == 1) {
    PN = int(pLFMparams.PN);
    signal_length = mLFMs.cols();
    pulse_number  = (PN + 2) * mLFMs.cols();
  }else if (type == 2) {
    PN = 1;
    signal_length = mLFMs.cols();
    pulse_number  = (PN + 2) * mLFMs.cols();
  }
  
 	Initialize_ADRV9025(PN, signal_length, pulse_number, trxcfg); 
 
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
  
  if (_USE_CYLIC_BUFFER_){ printf("Use The Cylic Buffer\n"); }
  else { printf("Use non-Cylic Buffer\n"); }
  
  printf("* Starting IO streaming (press CTRL+C to cancel)\n");
  printf("*** Using %s Mode *** \n", type==1? "Pulse Radar" : "FMCW Radar or LoL Test" );
  bool _new_data_ = true;
  std::thread _thread_;
  
  if (type == 0) {
    _thread_ = std::thread(Send_Recv, _data_tx_, _data_rx_, std::ref(_new_data_));
  } else if (type == 1) {
    _thread_ = std::thread(Send_Recv_PulseRadar, _data_tx_, _data_rx_, std::ref(_new_data_));
  } else if (type == 2) {
    _thread_ = std::thread(Send_Recv_LoL, _data_tx_, _data_rx_);
  }
  _thread_.detach();
  
	while (!stop) {
     usleep(100);
	}
 
  _thread_.join();
  delete [] _data_tx_;
  delete [] _data_rx_;
  // DMA DeInitial
  dma_deinitial(chan_tx, chan_rx);
  // UDP DeInitial
  close_socket(sockfd);
	shutdown();
	return 0;
}
