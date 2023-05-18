// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <pybind11/functional.h>

#include "jpathgen/integration.h"
#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;
using namespace jpathgen::integration;
using namespace jpathgen::function;

PYBIND11_MODULE(_libjpathgen, m)
{
  m.doc() = "A C++ library to speed up jpathgen computations";

  m.def(
      "integrate_over_buffered_line",
      static_cast<double (*)(Function, std::vector<std::pair<double, double>>, double)>(&integrate_over_buffered_line));
  m.def(
      "integrate_over_buffered_line",
      static_cast<double (*)(Function, Eigen::Matrix<double, 2, Eigen::Dynamic>, double)>(&integrate_over_buffered_line));
}