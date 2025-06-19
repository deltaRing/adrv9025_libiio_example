#include "../../include/Algorithms/RadarFunctions.h"

extern bool stop;
extern int check_RX, check_TX;
extern int signal_length; // mLFM.cols()
extern int pulse_number; // all pulse number
extern int check_acTx; // actual emit
extern struct iio_buffer * rxbuf;
extern struct iio_buffer * txbuf;

// LFM define
extern MIMO_LFM_conf conf;

// Pulse LFM define
extern PulseRadar pLFMparams;

// PROCESS define
extern process_param params;

// Cancellation define
extern CancellationParam cparams;

// DMA define
extern std::vector<struct channel *> chan_tx, chan_rx;

// UDP define
extern struct sockaddr_in serverAddr;
extern int sockfd;

// Transfer and Receive 
void Send_Recv(int16_t * _data_tx_, int16_t * _data_rx_, bool & new_data) {
  ssize_t nbytes_tx, nbytes_rx;
  int index = 0, count = 0;
  
  Eigen::ArrayXXcf data(check_RX, pulse_number);
  Eigen::ArrayXXcf data_fast(check_RX, int(conf.PulseNum) * int(conf.SignalLength)); // 4 x tp
  Eigen::ArrayXXcf mix_data(check_RX - 1, int(conf.PulseNum) * int(conf.SignalLength)); // mixed with channal 1
  Eigen::ArrayXXcf PulseCompressed(int(conf.PulseNum), _RFFT_NUM_); // FFTN
  Eigen::ArrayXXcf MTD(_DFFT_NUM_, _RFFT_NUM_);
  Eigen::ArrayXXcf mtd(_DFFT_NUM_, _RFFT_NUM_);
               
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
    
    // GetData_MultiThread(data, rxbuf, 4);
    // Reduce Time
    // int startindex = PulseDetection_LowPulse(data, check_acTx, signal_length / check_acTx);
    
    load_iio_data_multi(data_fast, mix_data, rxbuf, int(conf.PulseNum),
                        int(conf.SignalLength), int(conf.PulseLength), check_RX, 0);
                        
    /*std::ofstream rr(std::to_string(count) + "rr.txt"), ii(std::to_string(count++) + "ii.txt");
    rr << data_fast.real();
    ii << data_fast.imag();
    rr.close();
    ii.close();
    
    exit(0);*/
       
    // PulseCompressed
    GetFFTData(PulseCompressed, mix_data, chan_tx, chan_rx, _RFFT_NUM_,
      conf, check_acTx, params);
        
    // Get MTD   
    GetFFTData(MTD, PulseCompressed, chan_tx, chan_rx, _DFFT_NUM_,
      conf, check_acTx, params);
    
    // 2 Dim-FFTShift
    mtd.block(0, 0, _DFFT_NUM_ / 2, _RFFT_NUM_) = MTD.block(_DFFT_NUM_ / 2 - 1, 0, _DFFT_NUM_ / 2, _RFFT_NUM_);
    mtd.block(_DFFT_NUM_ / 2 - 1, 0, _DFFT_NUM_ / 2, _RFFT_NUM_) = MTD.block(0, 0, _DFFT_NUM_ / 2, _RFFT_NUM_);
    
    auto end = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = end - start; // too slow!
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;
        
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


void Send_Recv_PulseRadar (int16_t * _data_tx_, int16_t * _data_rx_, bool & new_data) {
  ssize_t nbytes_tx, nbytes_rx;
  int index = 0, count = 0;
  
  Eigen::ArrayXXcf data(check_RX, pulse_number);
  Eigen::ArrayXXcf PulseCompressed(int(pLFMparams.PN), _RPFFT_NUM_); // FFTN
  Eigen::ArrayXXcf MTD(_DPFFT_NUM_, _RPFFT_NUM_);
  
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
      
    auto start = std::chrono::high_resolution_clock::now();
      
    GetData_MultiThread(data, rxbuf, 4);
      
    // Reduce Time
    int startindex = PulseDetection_LowPulse(data, check_acTx, signal_length / check_acTx);
    
    std::cout << startindex << std::endl;
    
    // Pulse Compression
    PulseCompression(data, startindex,
               pLFMparams, PulseCompressed, chan_tx, chan_rx, 1);
    
    // MTD
    MotionTargetDetection(pLFMparams, PulseCompressed, MTD);
 
    auto end = std::chrono::high_resolution_clock::now(); 
    
    std::chrono::duration<double> elapsed = end - start; // too slow!
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;
    
    // TODO: Send UDP Data
    // TCP    
    for (int ii = 0; ii < MTD.cols(); ii++) {
      // Send Data
      if (sockfd > 0)
        send_data(serverAddr, sockfd, (unsigned char *)&ii, sizeof(int));
      else
        printf("***** WARNING: SOCKFD < 0, Bind Failed! *****\n");
      // usleep(1000);
    
      // Send Data
      if (sockfd > 0)
        send_data(serverAddr, sockfd, (unsigned char *)MTD.col(ii).data(), 
          sizeof(std::complex<float>) * MTD.rows());
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
    
    std::ofstream rr(std::to_string(count) + "rr.txt"), ii(std::to_string(count++) + "ii.txt");
    rr << data.real();
    ii << data.imag();
    rr.close();
    ii.close();
    
    if (count > 100) {
      exit(0);
    }
    
    
    auto end = std::chrono::high_resolution_clock::now(); 
    
    std::chrono::duration<double> elapsed = end - start; // too slow!
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;
  }
}