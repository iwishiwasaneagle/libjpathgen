// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#ifndef JDRONES_INTEGRATION_H
#define JDRONES_INTEGRATION_H

#include <cubpackpp/cubpackpp.h>

#include <Eigen/Core>

#include "jpathgen/function.h"

#define DEFAULT_D 2.5

namespace jpathgen
{
  namespace integration
  {
    double
    integrate_over_buffered_line(function::Function f, std::vector<std::pair<double, double>> coords, double d = DEFAULT_D);
    double integrate_over_buffered_line(
        function::Function f,
        Eigen::Matrix<double, 2, Eigen::Dynamic> coords,
        double d = DEFAULT_D);
  }  // namespace integration
}  // namespace jpathgen
#endif  // JDRONES_INTEGRATION_H
