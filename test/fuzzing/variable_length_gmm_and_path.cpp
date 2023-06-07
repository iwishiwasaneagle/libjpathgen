// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <cifuzz/cifuzz.h>
#include <fuzzer/FuzzedDataProvider.h>
#include "jpathgen/geometry.h"
#include "jpathgen/environment.h"
#include "jpathgen/integration.h"

using namespace jpathgen::geometry;
using namespace jpathgen::environment;
using namespace jpathgen::integration;

FUZZ_TEST_SETUP() {
}

FUZZ_TEST(const uint8_t *data, size_t size) {
  FuzzedDataProvider fuzzed_data(data, size);

  MUS mus = MUS::Zero(1,2);
  COVS covs = COVS::Zero(2,2);
  for(int i=0;i<4;i++){

    COV cov = COV::Zero();
    MU mu;

    mu(0,0) = fuzzed_data.ConsumeFloatingPointInRange(-5000.,5000.);
    mu(0,1) = fuzzed_data.ConsumeFloatingPointInRange(-5000.,5000.);

    cov(0,0) = fuzzed_data.ConsumeFloatingPointInRange(1e-6,5000.);
    cov(1,1) = fuzzed_data.ConsumeFloatingPointInRange(1e-6,5000.);

    mus.block<1,2>(i,0) = mu;
    covs.block<2,2>(i*2,0) = cov;
  }
  MultiModalBivariateGaussian gmm(mus, covs);

  int N_wps = fuzzed_data.ConsumeIntegralInRange(2,100);
  EigenCoords coords = EigenCoords::Random(N_wps, 2);

  for(int i=0;i<N_wps;i++)
  {
    Eigen::Vector2d coord;
    coord(0,0) = fuzzed_data.ConsumeFloatingPointInRange(-10000.,10000.);
    coord(1,0) = fuzzed_data.ConsumeFloatingPointInRange(-10000.,10000.);

    coords.block<1,2>(i,0) = coord;
  }
  double buffer_width = fuzzed_data.ConsumeFloatingPointInRange(1e-3,100.);
  double result = integrate_over_buffered_line(gmm, coords, buffer_width);

  assert(result>=0);
}
