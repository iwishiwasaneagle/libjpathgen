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

namespace jpathgen
{
  namespace integration
  {
    template<typename FUNC>
    double _conversion(FUNC f, const cubpackpp::Point& pt)
    {
      double x = pt.X(), y = pt.Y();
      return f(x, y);
    }

    double
    _integration_over_region_collections(cubpackpp::Function fn,cubpackpp::REGION_COLLECTION rc){
      return cubpackpp::Integrate(fn, rc, 0, 0.05);
    }

    template<typename FUNC, typename COORDS>
    double integrate_over_buffered_line(FUNC f, COORDS coords, double d)
    {
      std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), d);
      auto triangulated = geometry::triangulate_polygon(std::move(buffered));
      cubpackpp::REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);
      cubpackpp::Function fn_bound = std::bind(&_conversion<FUNC>, f, std::placeholders::_1);
      return _integration_over_region_collections(fn_bound, rg);
    }
    template double integrate_over_buffered_line(function::Function, geometry::EigenCoords, double);
    template double integrate_over_buffered_line(function::Function, geometry::STLCoords, double);
    template double integrate_over_buffered_line(environment::MultiModalBivariateGaussian, geometry::EigenCoords, double);
    template double integrate_over_buffered_line(environment::MultiModalBivariateGaussian, geometry::STLCoords, double);
    template double integrate_over_buffered_line(double (*)(double, double), geometry::EigenCoords, double);
    template double integrate_over_buffered_line(double (*)(double, double), geometry::STLCoords, double);

    template<typename FUNC>
    double integrate_over_rect(FUNC f, double left, double right, double bottom, double top)
    {
      cubpackpp::REGION_COLLECTION rc;
      cubpackpp::Point A(left, bottom), B(left, top), C(right, bottom);
      cubpackpp::RECTANGLE rect(A, B, C);
      rc += rect;

      cubpackpp::Function fn_bound = std::bind(&_conversion<FUNC>, f, std::placeholders::_1);

      return _integration_over_region_collections(fn_bound, rc);
    }
    template double integrate_over_rect(environment::MultiModalBivariateGaussian, double, double, double, double);
    template double integrate_over_rect(function::Function, double, double, double, double);

  }  // namespace integration
}  // namespace jpathgen