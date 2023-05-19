// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/environment.h"
#include <iostream>

using namespace jpathgen::environment;

void tmp(){};

int main()
{
  STLMUS mus = {{0,0},{2,2},{3,3},{4,4}};
  STLCOVS covs = {{1,0},{0,1},{1,0},{0,1},{1,0},{0,1},{1,0},{0,1}};
  MultiModalBivariateGaussian mmbg(mus, covs);
  Eigen::MatrixXd x , y;
  x << 1,2,3,4;
  y << 1,2,3,4;

  double sum;
  for (int i=0; i<100; i++){
    sum = mmbg(x,y).array().sum();
  }
  std::cout << sum <<std::endl;
  tmp();
};
