#include "../../include/Utilize/data_convert.h"

void GetDataSubProcess(Eigen::ArrayXXcf & stored_data, int start_index, 
      int enable_channel_num, char * start_address, char * end_address, ptrdiff_t p_inc){
  if (start_index < 0 || start_index > stored_data.cols()){
    assert(!"GetDataSubProcess: start index is error");
  }
  if (start_address == NULL){
    assert(!"GetDataSubProcess: start address is NULL pointer");
  }
  if (end_address == NULL){
    assert(!"GetDataSubProcess: end address is NULL pointer");
  }
  
  char *p_dat;
  for (p_dat = (char *)start_address; p_dat < (char *)end_address; p_dat += p_inc){
    if (enable_channel_num >= 1){
      stored_data(0, start_index) = std::complex<float>{ ((int16_t*)p_dat)[0], ((int16_t*)p_dat)[1] };  
    }
    if (enable_channel_num >= 2){
      stored_data(1, start_index) = std::complex<float>{ ((int16_t*)p_dat)[2], ((int16_t*)p_dat)[3] };  
    }
    if (enable_channel_num >= 3){
      stored_data(2, start_index) = std::complex<float>{ ((int16_t*)p_dat)[4], ((int16_t*)p_dat)[5] };  
    }
    if (enable_channel_num >= 4){
      stored_data(3, start_index) = std::complex<float>{ ((int16_t*)p_dat)[6], ((int16_t*)p_dat)[7] };     
    }
    start_index++;
  }
}

void GetData(Eigen::ArrayXXcf & stored_data, struct iio_buffer * rx_buf){
  int enable_channel_num = stored_data.rows(); // Get channels
  int sample_num = stored_data.cols();         // Get Sample Numbers
  
  char *p_dat, *p_end;
	ptrdiff_t p_inc;
  p_inc = iio_buffer_step(rx_buf);
  p_end = (char *)iio_buffer_end(rx_buf);
  
  int ii = 0;
  for (p_dat = (char *)iio_buffer_start(rx_buf); p_dat < p_end; p_dat += p_inc){
    if (enable_channel_num >= 1){
      stored_data(0, ii) = std::complex<float>{ static_cast<float>(((int16_t*)p_dat)[0]), static_cast<float>(((int16_t*)p_dat)[1]) };  
    }
    if (enable_channel_num >= 2){
      stored_data(1, ii) = std::complex<float>{ static_cast<float>(((int16_t*)p_dat)[2]), static_cast<float>(((int16_t*)p_dat)[3]) };  
    }
    if (enable_channel_num >= 3){
      stored_data(2, ii) = std::complex<float>{ static_cast<float>(((int16_t*)p_dat)[4]), static_cast<float>(((int16_t*)p_dat)[5]) };  
    }
    if (enable_channel_num >= 4){
      stored_data(3, ii) = std::complex<float>{ static_cast<float>(((int16_t*)p_dat)[6]), static_cast<float>(((int16_t*)p_dat)[7]) };     
    }
    ii++;
  }
}

void GetData_MultiThread(Eigen::ArrayXXcf & data, struct iio_buffer * rx_buf, int ThreadNumber){
  if (rx_buf == NULL){
    assert(!"GetData_MultiThread: rx_buf is NULL pointer");
  }
  if (ThreadNumber <= 0){
    assert(!"GetData_MultiThread: ThreadNumber Should be greater than zero");
  }
  

  int enable_channel_num = data.rows(); // Get channels
  int sample_num = data.cols();         // Get Sample Numbers
  
  char *p_start, *p_end; // ��ʼָ���Լ�����ָ��
	ptrdiff_t p_inc;       // buffer ����
  p_inc = iio_buffer_step(rx_buf);
  
  int data_size   = (int)iio_buffer_end(rx_buf) - 
                    (int)iio_buffer_start(rx_buf); // �ֽ�ƫ����
  int expect_step = data_size / ThreadNumber;      // 
  int expect_data_step = data_size / sizeof(int) / enable_channel_num / ThreadNumber; // ����ָ��
  
  std::thread * _threads_ = new std::thread[ThreadNumber];
  
  for (int ii = 0; ii < ThreadNumber; ii++){
  
    if (ii < ThreadNumber - 1){
      p_start = (char *)(iio_buffer_start(rx_buf) + ii * expect_step);
      p_end = (char *)(iio_buffer_start(rx_buf) + (ii + 1) * expect_step);
    
      _threads_[ii] = std::thread(GetDataSubProcess, std::ref(data), expect_data_step * ii, 
          enable_channel_num, p_start, p_end, p_inc);
    }else{
      // p_end ֱ����end��ʾ
      p_start = (char *)(iio_buffer_start(rx_buf) + ii * expect_step);
      p_end = (char *)iio_buffer_end(rx_buf);
      
      _threads_[ii] = std::thread(GetDataSubProcess, std::ref(data), expect_data_step * ii, 
          enable_channel_num, p_start, p_end, p_inc);
    }
  }
  
  // �ȴ�ѭ������
  for (int ii = 0; ii < ThreadNumber; ii++){
    _threads_[ii].join();
  }
  
  // check thread finished
  delete [] _threads_;
}

void GetData16(std::complex<int16_t *> ** data, int enable_channel_num, int sample_num, 
      struct iio_buffer * rx_buf){
  char *p_dat, *p_end;
	ptrdiff_t p_inc;
  p_inc = iio_buffer_step(rx_buf);
  p_end = (char *)iio_buffer_end(rx_buf);
  
  int ii = 0;
  int16_t * address = (int16_t *)iio_buffer_start(rx_buf);
  
  for (p_dat = (char *)iio_buffer_start(rx_buf); p_dat < p_end; p_dat += p_inc){
    if (enable_channel_num >= 1){
      data[0][ii] = std::complex<int16_t *> {address + ii * p_inc, address + 2 + ii * p_inc};
    }
    if (enable_channel_num >= 2){
      data[1][ii] = std::complex<int16_t *> {address + 4 + ii * p_inc, address + 6 + ii * p_inc}; 
    }
    if (enable_channel_num >= 3){
      data[2][ii] = std::complex<int16_t *> {address + 8 + ii * p_inc, address + 10 + ii * p_inc};
    }
    if (enable_channel_num >= 4){
      data[3][ii] = std::complex<int16_t *> {address + 12 + ii * p_inc, address + 14 + ii * p_inc};   
    }
    ii++;
  }
}
