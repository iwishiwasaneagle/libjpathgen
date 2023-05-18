// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/integration.h"

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateArraySequence.h>

#include <cmath>

#include "jpathgen/geometry.h"
#include "jpathgen/integration.h"

using namespace geos::geom;
using namespace geos::triangulate::tri;
using cubpackpp::real;
using cubpackpp::REGION_COLLECTION;
using cubpackpp::TRIANGLE;
using Pt = cubpackpp::Point;

namespace jpathgen
{
  namespace integration
  {
    inline double _conversion(function::Function f, const cubpackpp::Point& pt)
    {
      double x = pt.X(), y = pt.Y();
      return f(x, y);
    }

    inline double
    _integration_over_buffered_line(function::Function f, std::unique_ptr<CoordinateArraySequence> cs, double d)
    {
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), d);
      auto triangulated = geometry::triangulate_polygon(std::move(buffered));
      REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);

      cubpackpp::Function fn_bound = std::bind(&_conversion, f, std::placeholders::_1);

      return cubpackpp::Integrate(fn_bound, rg, 0, 0.05);
    }

    double integrate_over_buffered_line(function::Function f, std::vector<std::pair<double, double>> coords, double d)
    {
      auto cs = geometry::coord_sequence_from_array(coords);
      return _integration_over_buffered_line(f, std::move(cs), d);
    }

    double integrate_over_buffered_line(function::Function f, Eigen::Matrix<double, 2, Eigen::Dynamic> coords, double d)
    {
      auto cs = geometry::coord_sequence_from_array(coords);
      return _integration_over_buffered_line(f, std::move(cs), d);
    }
  }  // namespace integration
}  // namespace jpathgen