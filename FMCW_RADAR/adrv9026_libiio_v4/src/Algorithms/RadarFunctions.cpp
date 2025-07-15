#include "../../include/Algorithms/RadarFunctions.h"

extern bool stop;
extern int check_RX, check_TX;
extern int signal_length; // mLFM.cols()
extern int pulse_number; // all pulse number
extern int check_acTx; // actual emit
extern struct iio_buffer * rxbuf;
extern struct iio_buffer * txbuf;
extern std::vector<struct channel *> chan_tx, chan_rx;

// LFM define
extern MIMO_LFM_conf conf;

// Pulse LFM define
extern PulseRadar pLFMparams;

// PROCESS define
extern process_param params;

// CFAR define
extern x1d_cacfar_param xparams;

// DMA define
extern std::vector<struct channel *> chan_tx, chan_rx;

// UDP define
extern struct sockaddr_in serverAddr;
extern int sockfd;

// Transfer and Receive 
void Send_Recv(int16_t * _data_tx_, int16_t * _data_rx_, bool & new_data) {
  ssize_t nbytes_tx, nbytes_rx;
  int index = 0, count = 0;
  
  Eigen::ArrayXXcf PulseCompressed(int(conf.PulseNum), _RFFT_NUM_); // FFTN
  Eigen::ArrayXXcf MTD(_DFFT_NUM_, _RFFT_NUM_);
  Eigen::ArrayXXcf mtd(_DFFT_NUM_, _RFFT_NUM_);
  
  // get the interval
  int interval = params.ori_fs / params.ex_fs;
  int out_len  = conf.PulseLength / interval;
  // Get the FFTN
  const int FFTN = 11;
  if (std::pow(2, FFTN) < int(conf.SignalLength) / interval) printf(" *** Critical Warning: Pulse Length is Greater than FFT Length!!! *** \n");
  // receive buffer
  Eigen::ArrayXXcf data_buffer(check_RX, int(conf.PulseNum) * int(std::pow(2, FFTN))); // 4 x tp
  Eigen::ArrayXXcf mix_buffer(check_RX - 1, int(conf.PulseNum) * int(std::pow(2, FFTN))); // mixed with channal 1
               
  // sleep(10);
  while (!stop)
	{
#if (_USE_CYLIC_BUFFER_ == 0)
    if (new_data) {
       for (int ii = 0; ii < _DMA_Number_; ii++){
          uint64_t length = (long)iio_buffer_end(txbuf) - (long)iio_buffer_start(txbuf);
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
          uint64_t length = (uint64_t)iio_buffer_end(txbuf) - (uint64_t)iio_buffer_start(txbuf);
          // copy buffer
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
    uint64_t length = (uint64_t)iio_buffer_end(rxbuf) - (uint64_t)iio_buffer_start(rxbuf);
    
    if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int)nbytes_rx); shutdown(); }
    else { }
    if (index < 100) { index++; usleep(100); continue; }
    
    auto start_pulse = std::chrono::high_resolution_clock::now();
    
    
    // Reduce Time
    load_iio_data_multi_fmcw(data_buffer, mix_buffer, rxbuf, interval, int(std::pow(2, FFTN)), int(conf.PulseNum), 
                        int(conf.SignalLength), int(conf.PulseLength), check_RX, 0);
       
    // PulseCompressed
    PulseCompression_FMCW(PulseCompressed, mix_buffer, 
              chan_tx, chan_rx, 
              conf, params);
        
    
    auto start_mtd = std::chrono::high_resolution_clock::now(); 
    
    // Get MTD   
    MotionTargetDetection_FMCW(MTD, PulseCompressed, conf, params);
    
    // 2 Dim-FFTShift
    mtd.block(0, 0, _DFFT_NUM_ / 2, _RFFT_NUM_) = MTD.block(_DFFT_NUM_ / 2 - 1, 0, _DFFT_NUM_ / 2, _RFFT_NUM_);
    mtd.block(_DFFT_NUM_ / 2 - 1, 0, _DFFT_NUM_ / 2, _RFFT_NUM_) = MTD.block(0, 0, _DFFT_NUM_ / 2, _RFFT_NUM_);
    
    
    // Get CFAR
    
    auto end = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed1 = start_mtd - start_pulse, elapsed2 = end - start_mtd; // too slow!
    std::cout << "Time elapsed: " << elapsed1.count() << " seconds " << elapsed2.count() << " seconds" << std::endl;
        
    // TCP    
    for (int ii = 0; ii < mtd.cols(); ii++) {
      // Send Data
      if (sockfd > 0)
        send_data(serverAddr, sockfd, (unsigned char *)&ii, sizeof(int));
      else
        printf("***** WARNING: SOCKFD < 0, Bind Failed! *****\n");
      // usleep(1000);
    
      // Send Data
      if (sockfd > 0)
        send_data(serverAddr, sockfd, (unsigned char *)mtd.col(ii).data(), 
          sizeof(std::complex<float>) * mtd.rows());
      else
        printf("***** WARNING: SOCKFD < 0, Bind Failed! *****\n");
    }
  }
}

void PulseCompression_Subthread(Eigen::ArrayXXcf & pc) {
    // Reduce Time
    int startindex = PulseDetection_LowPulse(rxbuf, 0, pLFMparams.siglen, check_RX);
    
    // DMA Pulse Compression
    dma_pulse_compression(rxbuf, pc,
                  chan_tx, chan_rx,
                  startindex, pLFMparams);
}

void MTD_Subthread(Eigen::ArrayXXcf pc, Eigen::ArrayXXcf & MTD, Eigen::ArrayXXcf & mtd) {
    // MTD
    MotionTargetDetection(pLFMparams, pc, MTD);
    
    // 2 Dim-FFTShift
    mtd.block(0, 0, _DPFFT_NUM_ / 2, _RPFFT_NUM_) = MTD.block(_DPFFT_NUM_ / 2 - 1, 0, _DPFFT_NUM_ / 2, _RPFFT_NUM_);
    mtd.block(_DPFFT_NUM_ / 2 - 1, 0, _DPFFT_NUM_ / 2, _RPFFT_NUM_) = MTD.block(0, 0, _DPFFT_NUM_ / 2, _RPFFT_NUM_);
}

void Send_Recv_PulseRadar(int16_t * _data_tx_, int16_t * _data_rx_, bool & new_data) {
  ssize_t nbytes_tx, nbytes_rx;
  int index = 0, count = 0;
  
  Eigen::ArrayXXcf data(check_RX, pulse_number);
  Eigen::ArrayXXcf PulseCompressed(int(pLFMparams.PN), _RPFFT_NUM_); // FFTN
  Eigen::ArrayXXcf pc(int(pLFMparams.PN), _RPFFT_NUM_); // FFTN
  Eigen::ArrayXXcf MTD(_DPFFT_NUM_, _RPFFT_NUM_);
  Eigen::ArrayXXcf mtd(_DPFFT_NUM_, _RPFFT_NUM_); // 
  bool _done_flag_[2] = {true, false};
  
  if (pLFMparams.prilen > _RPFFT_NUM_) {
    printf(" *** Critical Warning: The PRI Length is larger than FFTN, Select a Small One *** \n");
    exit(-1);
  }
  
  // Set the Receive Channel
  dma_set_channel(1);
  
  while (!stop) {
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
    if (index < 100) { index++; usleep(100); continue; }
      
    auto start_pulse = std::chrono::high_resolution_clock::now();
    
    std::thread _thread_[2]; // 0: pulse thread 1: mtd thread
    
    
    if (_done_flag_[1]) {
      _thread_[1] = std::thread(MTD_Subthread, pc, std::ref(MTD), std::ref(mtd));
    }
    
    // Complete 
    _thread_[0] = std::thread(PulseCompression_Subthread, std::ref(PulseCompressed));
    
    for (int ii = 0; ii < 2; ii++) {
      if (_done_flag_[ii])
        _thread_[ii].join();
    }
    
    auto end = std::chrono::high_resolution_clock::now(); 
    
    _done_flag_[1] = true;
    std::chrono::duration<double> elapsed = end - start_pulse; // too slow!
    std::cout << "Time elapsed: " << elapsed.count() << " seconds " << std::endl;
    pc = PulseCompressed;
    
    // TODO: Send UDP Data
    // TCP    
    for (int ii = 0; ii < mtd.cols(); ii++) {
      // Send Data
      if (sockfd > 0)
        send_data(serverAddr, sockfd, (unsigned char *)&ii, sizeof(int));
      else
        printf("***** WARNING: SOCKFD < 0, Bind Failed! *****\n");
      // usleep(1000);
    
      // Send Data
      if (sockfd > 0)
        send_data(serverAddr, sockfd, (unsigned char *)mtd.col(ii).data(), 
          sizeof(std::complex<float>) * mtd.rows());
      else
        printf("***** WARNING: SOCKFD < 0, Bind Failed! *****\n");
    }
    
  }
}

void Send_Recv_LoL(int16_t * _data_tx_, int16_t * _data_rx_) {
  ssize_t nbytes_tx, nbytes_rx;
  int index = 0, count = 0;
  bool new_data = true;
  
  Eigen::ArrayXXcf data(check_RX, pulse_number);
               
  // sleep(10);
  while (!stop)
	{
#if (_USE_CYLIC_BUFFER_ == 0)
    if (new_data) {
       for (int ii = 0; ii < _DMA_Number_; ii++){
          uint64_t length = (long)iio_buffer_end(txbuf) - (long)iio_buffer_start(txbuf);
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
          uint64_t length = (uint64_t)iio_buffer_end(txbuf) - (uint64_t)iio_buffer_start(txbuf);
          // copy buffer
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
    uint64_t length = (uint64_t)iio_buffer_end(rxbuf) - (uint64_t)iio_buffer_start(rxbuf);
    
    if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int)nbytes_rx); shutdown(); }
    else { }
    if (index < 100) { index++; usleep(100); continue; }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    GetData_MultiThread(data, rxbuf, 4);
    
    if (count > 100) {
      exit(0);
    }
  
    auto end = std::chrono::high_resolution_clock::now(); 
    
    std::chrono::duration<double> elapsed = end - start; // too slow!
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;
  }
}