// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/integration.h"

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateArraySequence.h>

#include <cmath>
#include <functional>

#include "jpathgen/environment.h"
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
    template<typename Callable>
    double _conversion(Callable f, const cubpackpp::Point& pt)
    {
      double x = pt.X(), y = pt.Y();
      return f(x, y);
    }

    template<typename Callable>
    double
    _integration_over_buffered_line(Callable g, std::unique_ptr<CoordinateArraySequence> cs, double d)
    {
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), d);
      auto triangulated = geometry::triangulate_polygon(std::move(buffered));
      REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);
      cubpackpp::Function fn_bound = std::bind(&_conversion<Callable>, g, std::placeholders::_1);

      return cubpackpp::Integrate(fn_bound, rg, 0, 0.05);
    }

    template<typename Callable, typename Coords>
    double integrate_over_buffered_line(Callable f, Coords coords, double d)
    {
      auto cs = geometry::coord_sequence_from_array(coords);
      return _integration_over_buffered_line(f, std::move(cs), d);
    }
    double integrate_over_buffered_line(function::Function f, geometry::EigenCoords coords, double d)

      {
        auto cs = geometry::coord_sequence_from_array(coords);
        return _integration_over_buffered_line(f, std::move(cs), d);
      }

    double integrate_over_buffered_line(function::Function f, geometry::STLCoords coords, double d)
    {
      auto cs = geometry::coord_sequence_from_array(coords);
      return _integration_over_buffered_line(f, std::move(cs), d);
    }

    double integrate_over_buffered_line(environment::MultiModalBivariateGaussian g, geometry::EigenCoords coords, double d)
    {
      auto cs = geometry::coord_sequence_from_array(coords);
      return _integration_over_buffered_line(g, std::move(cs), d);
    }

    double integrate_over_buffered_line(environment::MultiModalBivariateGaussian g, geometry::STLCoords coords, double d)
    {
      auto cs = geometry::coord_sequence_from_array(coords);
      return _integration_over_buffered_line(g, std::move(cs), d);
    }

  }  // namespace integration
}  // namespace jpathgen