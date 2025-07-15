#include "../../include/Utilize/fast_load_data.h"

void write_iio_data_bin(std::string name, struct iio_buffer * rx_buf){
  std::ofstream outFile(name.c_str(), std::ios::binary);
  if (!outFile) {
    std::cerr << "Can not Write to " << name << "\n";
    return;
  }

  char *p_dat, *p_end;
	ptrdiff_t p_inc;
  p_inc = iio_buffer_step(rx_buf);
  p_dat = (char *)iio_buffer_start(rx_buf);
  p_end = (char *)iio_buffer_end(rx_buf);
  
  std::cout << "Data Length: " << uint32_t(p_end - p_dat) << std::endl;
  
  outFile.write(reinterpret_cast<char*>(p_dat), uint32_t(p_end - p_dat));
  outFile.close();
  
  std::cout << "Save Complete... \n";
}

void load_iio_data(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, struct iio_buffer * rx_buf, int pulse_num, 
                              int tpLen, int sigLen, int enable_chan, int loop_chan) {
  // get the pulse location
  int start_offset = PulseDetection_LowPulse(rx_buf, loop_chan, sigLen, enable_chan);
  int sigLen_byteinterval = sigLen * enable_chan * sizeof(std::complex<short>), // 4 * 4 * xxxx
      tp_byteinterval     = enable_chan * sizeof(std::complex<short>),
      chan_byteinterval   = sizeof(std::complex<short>);
  
  char * start_address = (char *)iio_buffer_start(rx_buf); // related to IQ0
  
  for (int pp = 0; pp < pulse_num; pp++) {
    for (int tt = 0; tt < tpLen; tt++) {
      for (int rr = 0; rr < enable_chan; rr++) {
        short real_0 = 0, imag_0 = 0;
        std::memcpy(&real_0, start_address + start_offset + pp * sigLen_byteinterval + tt * tp_byteinterval + chan_byteinterval * rr, 2);
        std::memcpy(&imag_0, start_address + start_offset + pp * sigLen_byteinterval + tt * tp_byteinterval + chan_byteinterval * rr + 2, 2);
        
        data(rr, pp * tpLen + tt) = std::complex<float>{ static_cast<float>(real_0), static_cast<float>(imag_0) };  
        if (rr != 0) mixed(rr-1, pp * tpLen + tt) = data(rr, pp * tpLen + tt) * std::conj(data(0, pp * tpLen + tt));
      }
    }
  }
}


void load_iio_data_sub_thread(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, char * start_address, int start_pp, int pp_length, 
  int start_offset, int tpLen, int enable_chan, int s_byi, int t_byi, int c_byi) {

  for (int pp = start_pp; pp < pp_length; pp++) {
    for (int tt = 0; tt < tpLen; tt++) {
      for (int rr = 0; rr < enable_chan; rr++) {
        short real_0 = 0, imag_0 = 0;
        std::memcpy(&real_0, start_address + start_offset + pp * s_byi + tt * t_byi + c_byi * rr, 2);
        std::memcpy(&imag_0, start_address + start_offset + pp * s_byi + tt * t_byi + c_byi * rr + 2, 2);
        
        data(rr, pp * tpLen + tt) = std::complex<float>{ static_cast<float>(real_0), static_cast<float>(imag_0) }; 
        if (rr != 0) mixed(rr-1, pp * tpLen + tt) = data(rr, pp * tpLen + tt) * std::conj(data(0, pp * tpLen + tt));
      }
    }
  }
  
}

void load_iio_data_multi(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, struct iio_buffer * rx_buf, int pulse_num, 
                              int tpLen, int sigLen, int enable_chan, int loop_chan) {
                        
  int start_offset = PulseDetection_LowPulse(rx_buf, loop_chan, sigLen, enable_chan);
  int sigLen_byteinterval = sigLen * enable_chan * sizeof(std::complex<short>), // 4 * 4 * xxxx
      tp_byteinterval     = enable_chan * sizeof(std::complex<short>),
      chan_byteinterval   = sizeof(std::complex<short>);
  
  // std::cout << start_offset << std::endl;
  
  char * start_address = (char *)iio_buffer_start(rx_buf); // related to IQ0
  
  std::thread _threads_[8];
  int length = pulse_num / 8;
  for (int ii = 0; ii < 8; ii++) {
    _threads_[ii] = std::thread(load_iio_data_sub_thread, std::ref(data), std::ref(mixed), start_address, length * ii, length * (ii + 1),
         start_offset, tpLen, enable_chan, sigLen_byteinterval, tp_byteinterval, chan_byteinterval);
  }
    
  for (int ii = 0; ii < 8; ii++) _threads_[ii].join();
}

void load_iio_data_sub_thread_fmcw(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, char * start_address, int start_pp, int pp_length, 
  int start_offset, int tpLen, int enable_chan, int s_byi, int t_byi, int c_byi, int down_sample, int single_length) {

  for (int pp = start_pp; pp < pp_length; pp++) {
    int tIndex = 0;
    for (int tt = 0; tt < tpLen; tt+=down_sample) {
      for (int rr = 0; rr < enable_chan; rr++) {
        short real_0 = 0, imag_0 = 0;
        std::memcpy(&real_0, start_address + start_offset + pp * s_byi + tt * t_byi + c_byi * rr, 2);
        std::memcpy(&imag_0, start_address + start_offset + pp * s_byi + tt * t_byi + c_byi * rr + 2, 2);
        
        data(rr, pp * single_length + tIndex) = std::complex<float>{ static_cast<float>(real_0), static_cast<float>(imag_0) }; 
        if (rr != 0) mixed(rr-1, pp * single_length + tIndex) = data(rr, pp * single_length + tIndex) * std::conj(data(0, pp * single_length + tIndex));
      }
      tIndex++;
    }
  }
  
}

void load_iio_data_multi_fmcw(Eigen::ArrayXXcf & data, Eigen::ArrayXXcf & mixed, struct iio_buffer * rx_buf, 
                        int down_sample_interval, int single_length, int pulse_num, int tpLen, int sigLen, int enable_chan, int loop_chan) {
                    int start_offset = PulseDetection_LowPulse(rx_buf, loop_chan, sigLen, enable_chan);
  int sigLen_byteinterval = sigLen * enable_chan * sizeof(std::complex<short>), // 4 * 4 * xxxx
      tp_byteinterval     = enable_chan * sizeof(std::complex<short>),
      chan_byteinterval   = sizeof(std::complex<short>);
  
  char * start_address = (char *)iio_buffer_start(rx_buf); // related to IQ0    
  
  std::thread _threads_[8];
  int length = pulse_num / 8;
  for (int ii = 0; ii < 8; ii++) {
    _threads_[ii] = std::thread(load_iio_data_sub_thread_fmcw, std::ref(data), std::ref(mixed), start_address, length * ii, length * (ii + 1),
         start_offset, tpLen, enable_chan, sigLen_byteinterval, tp_byteinterval, chan_byteinterval, down_sample_interval, single_length);
  }
    
  for (int ii = 0; ii < 8; ii++) _threads_[ii].join();
}

