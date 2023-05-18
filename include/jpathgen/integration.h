// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#ifndef JDRONES_INTEGRATION_H
#define JDRONES_INTEGRATION_H

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/CoordinateArraySequence.h>

#include <Eigen/Core>
#include <memory>

#include "jpathgen/environment.h"
#include "jpathgen/geometry.h"
#include "jpathgen/function.h"

#define DEFAULT_D 2.5

namespace jpathgen
{
  namespace integration
  {

    template<typename Callable>
    double _conversion(Callable f, const cubpackpp::Point& pt);

    template<typename T>
    double
    _integration_over_buffered_line(T g, std::unique_ptr<geos::geom::CoordinateArraySequence> cs, double d);

    double integrate_over_buffered_line(function::Function f, geometry::EigenCoords coords, double d = DEFAULT_D);
    double integrate_over_buffered_line(function::Function f, geometry::STLCoords coords, double d = DEFAULT_D);
    double integrate_over_buffered_line(environment::MultiModalBivariateGaussian g, geometry::EigenCoords coords, double d = DEFAULT_D);
    double integrate_over_buffered_line(environment::MultiModalBivariateGaussian g, geometry::STLCoords coords, double d = DEFAULT_D);
  }  // namespace integration
}  // namespace jpathgen
#endif  // JDRONES_INTEGRATION_H
