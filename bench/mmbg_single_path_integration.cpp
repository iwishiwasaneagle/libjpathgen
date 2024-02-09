/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <jpathgen/environment.h>
#include <jpathgen/geometry.h>
#include <jpathgen/integration.h>
#include <nanobench.h>

#include "generate.h"

void benchmark_discrete(
    ankerl::nanobench::Bench* bench,
    float integration_radius,
    jpathgen::geometry::EigenCoords coords,
    jpathgen::environment::MultiModalBivariateGaussian mmbg,
    int N,
    int M)
{
  std::stringstream name;
  name << "discrete_integration_over_mmbg";

  jpathgen::integration::DiscreteArgs* args = new jpathgen::integration::DiscreteArgs(
      integration_radius,
      N,
      M,
      coords.col(0).array().minCoeff(),
      coords.col(0).array().maxCoeff(),
      coords.col(1).array().minCoeff(),
      coords.col(1).array().maxCoeff());
  bench->context("N", std::to_string(N));
  bench->context("M", std::to_string(M));
  bench->run(name.str(), [&] { jpathgen::integration::discrete_integration_over_path(mmbg, coords, args); });
}
void benchmark_continuous(
    ankerl::nanobench::Bench* bench,
    float integration_radius,
    jpathgen::geometry::EigenCoords coords,
    jpathgen::environment::MultiModalBivariateGaussian mmbg)
{
  jpathgen::integration::ContinuousArgs* args = new jpathgen::integration::ContinuousArgs(integration_radius, 0.05, 0);
  bench->run(
      "continuous_integration_over_mmbg",
      [&] { jpathgen::integration::continuous_integration_over_path(mmbg, coords, args); });
}

int main()
{
  float integration_radius = 2.5;
  ankerl::nanobench::Bench b;
  b.title("Integration methods over single path + MMBG").unit("path").performanceCounters(true).relative(true);

  int minN = 5, maxN = 100, stepsN = 10;
  jpathgen::geometry::EigenCoords coords = generate_path(100);
  jpathgen::environment::MultiModalBivariateGaussian mmbg = generate_mmbg();

  b.context("N", "-1").context("M", "-1");
  benchmark_continuous(&b, integration_radius, coords, mmbg);

  for (int N = minN; N <= maxN; N += (maxN - minN) / stepsN)
  {
    benchmark_discrete(&b, integration_radius, coords, mmbg, N, N);
  }
}
