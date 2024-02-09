/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <jpathgen/environment.h>
#include <jpathgen/geometry.h>
#include <nanobench.h>

#include "generate.h"


int main()
{
  ankerl::nanobench::Bench b;
  b.title("MMBG").minEpochIterations(1e6).unit("mmbg").performanceCounters(true);
  jpathgen::environment::MultiModalBivariateGaussian mmbg = generate_mmbg(50);
  b.name("call mmbg with (double, double)").run([&] { mmbg(0.0, 0.0); });
  b.name("call mmbg with (int, int)").run([&] { mmbg(0, 0); });
}
