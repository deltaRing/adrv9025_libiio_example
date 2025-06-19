#include "Algorithms/DownSampling.h"

Eigen::ArrayXXcf downsampling(Eigen::ArrayXXcf fb, float ofs, float efs) {
  float interval = ofs / efs;
  int offset_start = 500;
  int offset_end   = 500;  
    
  Eigen::ArrayXXcf downsampled(1, int((fb.cols() - 1 - offset_start - offset_end) / interval));
  
  // std::cout << downsampled.cols() << std::endl;
  
  for (int ii = 0; ii < downsampled.cols(); ii++) {
    downsampled(0, ii) = fb(0, int(offset_start + ii * interval));
  }

  return downsampled;
}