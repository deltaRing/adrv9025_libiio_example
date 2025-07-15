#include "../../include/FMCW_MIMO/1D_CACFAR.h"

void X1D_CACFAR(Eigen::ArrayXXcf inMTD, x1d_cacfar_param param, 
  Eigen::ArrayXXf & outData, std::vector<Eigen::Vector3f> & detection) {
  
  int size_x = inMTD.rows(), size_y = inMTD.cols();
  int L_slipper = param.R + param.P;
  int L_slipper_P = param.P;
  float Pfa = param.Pfa;
  float ratio = pow(Pfa, (-1 / param.R / 2)) - 1;
  outData = Eigen::ArrayXXf::Zero(size_x, size_y);
  
  for (int ii = 0; ii < size_y; ii++) {
    for (int jj = L_slipper; jj < size_x - L_slipper; jj++) {
      int test_unit_rr = jj, end_unit_rr = jj + L_slipper;
      int test_unit_ps = test_unit_rr - L_slipper_P,
          test_unit_pe = test_unit_rr + L_slipper_P;
      // Get the test units
      Eigen::ArrayXXf test_units(1, 2 * L_slipper_P);
      for (int kk = test_unit_ps; kk < jj; kk++) {
        test_units(0, kk) = abs(inMTD(kk, ii));
      }
      
      for (int kk = test_unit_pe; kk < end_unit_rr; kk++) {
        test_units(0, kk) = abs(inMTD(kk, ii));
      }
      
      if (test_units.sum() * ratio < abs(inMTD(test_unit_rr, ii))) {
        outData(test_unit_rr, ii) = abs(inMTD(test_unit_rr, ii));
        Eigen::Vector3f res(test_unit_rr, ii, abs(inMTD(test_unit_rr, ii)));
        detection.push_back(res);
      }
    }
  }

}