// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <cubpackpp/cubpackpp.h>

#include <chrono>
#include <vector>

#include "jpathgen/integration.h"

using namespace jpathgen;

double f(const double& x, const double& y)
{
  return 1;
}

int main()
{
  int N = 100;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < N; i++)
  {
    std::vector<std::pair<double, double>> coords;
    for (float i = 0; i < 10 * M_PI; i += M_PI / 10)
    {
      std::pair<double, double> coord = { i * 4, cos(i) };
      coords.push_back(coord);
    }

    integration::integrate_over_buffered_line(f, coords, 2.5);
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

  std::cout << elapsed / N << "us per loop" << std::endl;
}
