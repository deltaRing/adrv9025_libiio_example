#ifndef _1D_CACFAR_H_
#define _1D_CACFAR_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

struct x1d_cacfar_param {
  float R = 8.0;
  float P = 6.0;
  float Pfa = 2.5e-2;
};

void X1D_CACFAR(Eigen::ArrayXXcf inMTD, x1d_cacfar_param param, Eigen::ArrayXXf & outData, std::vector<Eigen::Vector3f> & detection);


#endif