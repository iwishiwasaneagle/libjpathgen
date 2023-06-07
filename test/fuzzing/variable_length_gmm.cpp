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

  int N_gauss = fuzzed_data.ConsumeIntegralInRange(1,1000);

  MUS mus = MUS::Zero(N_gauss,2);
  COVS covs = COVS::Zero(N_gauss*2,2);
  for(int i=0;i<N_gauss;i++){

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
  double result = gmm(0,0);

  assert(result>=0);
}
