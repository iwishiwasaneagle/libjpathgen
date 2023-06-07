// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only
#include "CLI/CLI.hpp"
#include "jpathgen/geometry.h"
#include "jpathgen/environment.h"
#include "jpathgen/integration.h"

using namespace jpathgen::geometry;
using namespace jpathgen::environment;
using namespace jpathgen::integration;

bool run(STLMUS mus, STLCOVS covs,STLCoords coords, float buffer_width){
  MultiModalBivariateGaussian gmm(mus, covs);
  float result = integrate_over_buffered_line(gmm, coords, buffer_width);
  return result;
}

int main(int argc, char* argv[])
{
  CLI::App app{"CLI interface for jpathgen"};

  STLMUS mus;
  STLCOVS covs;

  app.add_option("-C,--covs", covs)->required()->expected(1,100);
  app.add_option("-M,--mus", mus)->required()->expected(1,100);

  STLCoords wps;
  app.add_option("wps", wps)->required()->expected(2, 1000);

  float buffer_width = 2.5;
  app.add_option("-b,--buffer-width", buffer_width)->check(CLI::Range(0.001,50.0));

  CLI11_PARSE(app, argc, argv);

  float result = run(mus, covs, wps, buffer_width);
  std::cout << result << std::endl;

  return !(result>=0);
}
