// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/integration.h"

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Coordinate.h>

#include "jpathgen/geos_compat.h"

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
using cubpackpp::RECTANGLE;
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

    double
    _integration_over_region_collections(cubpackpp::Function fn,cubpackpp::REGION_COLLECTION rc){
      return cubpackpp::Integrate(fn, rc, 0, 0.05);
    }

    template<typename Callable>
    double
    _integration_over_buffered_line(Callable g, std::unique_ptr<CoordinateSequenceCompat> cs, double d)
    {
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), d);
      auto triangulated = geometry::triangulate_polygon(std::move(buffered));
      REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);
      cubpackpp::Function fn_bound = std::bind(&_conversion<Callable>, g, std::placeholders::_1);

      return _integration_over_region_collections(fn_bound, rg);
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

    template<typename T>
    double
    _integrate_over_rect(T t, double left, double right, double bottom, double top){
      REGION_COLLECTION rc;
      Pt A(left,bottom), B(left, top), C(right, bottom);
      RECTANGLE rect(A, B, C);
      rc += rect;

      cubpackpp::Function fn_bound = std::bind(&_conversion<T>, t, std::placeholders::_1);

      return _integration_over_region_collections(fn_bound, rc);
    }
    double integrate_over_rect(environment::MultiModalBivariateGaussian g, double left, double right, double bottom, double top){
      return _integrate_over_rect(g, left, right, bottom, top);
    }
    double integrate_over_rect(function::Function f, double left, double right, double bottom, double top){
      return _integrate_over_rect(f, left, right, bottom, top);
    }

  }  // namespace integration
}  // namespace jpathgen