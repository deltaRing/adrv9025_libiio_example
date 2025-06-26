#ifndef _BACKGROUNDCANCELLATION_H_
#define _BACKGROUNDCANCELLATION_H_

// fftw
#include <fftw3.h>
// Eigen Matrix
#include <Eigen/Core>
#include <Eigen/Dense>
// Std defination
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
// user define
#include "../../include/FMCW_MIMO/FMCW_LFM.h"
#include "../../include/Utilize/common_define.h"

struct CancellationParam{
  int expand_num = 70;
  int init_cancel_num = 6;
  int separate_num = 10;
  float cancel_weight = 8.0;
};

// get the dft and idft matrix
Eigen::MatrixXcf GetDFTBack(int points);
Eigen::MatrixXcf GetIDFTBack(int points);

// Regular the amplify of the Interm Signal
void GetRegularIntermSignal(Eigen::ArrayXXcf & qInterm, 
                            Eigen::ArrayXXcf & inputSignal,
                            MIMO_LFM_conf config,
                            int start_index);

// update the user background cancellation params
void UpdateParams(Eigen::ArrayXXcf range_profile, CancellationParam param);
                            
//
void RangeProfileCancellation(Eigen::ArrayXXcf & range_profile);



#endif