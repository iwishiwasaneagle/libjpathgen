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
  COVS covs = COVS::Identity(2,2)*100;
  MultiModalBivariateGaussian gmm(mus, covs);

  int N_wps = fuzzed_data.ConsumeIntegralInRange(2,10000);
  EigenCoords coords = EigenCoords::Zero(N_wps, 2);

  for(int i=0;i<N_wps;i++)
  {
    Eigen::Vector2d coord;
    coord(0,0) = fuzzed_data.ConsumeFloatingPointInRange(-100.,100.);
    coord(1,0) = fuzzed_data.ConsumeFloatingPointInRange(-100.,100.);

    coords.block<1,2>(i,0) = coord;
  }
  double buffer_width = fuzzed_data.ConsumeFloatingPointInRange(1e-2,100.);
  double result = integrate_over_buffered_line(gmm, coords, buffer_width);

  // Errors creep into the system over large surfaces. Therefore, check if above some small negative value rather than zero
  assert(result>=-1e-10);
}
