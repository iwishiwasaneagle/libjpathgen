// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/integration.h"

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Coordinate.h>
#include <geos/operation/union/UnaryUnionOp.h>

#include <cmath>
#include <functional>
#include <utility>

#include "jpathgen/environment.h"
#include "jpathgen/geometry.h"
#include "jpathgen/geos_compat.h"
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

    template<typename FUNC>
    double integration_over_region_collections(FUNC f, cubpackpp::REGION_COLLECTION rc)
    {
      cubpackpp::Function fn_bound = std::bind(&_conversion<FUNC>, f, std::placeholders::_1);
      return integration_over_region_collections(fn_bound, rc);
    }
    template<>
    double integration_over_region_collections(cubpackpp::Function fn, cubpackpp::REGION_COLLECTION rc)
    {
      return cubpackpp::Integrate(fn, rc, 0, 0.05);
    }
    template double integration_over_region_collections(function::Function, cubpackpp::REGION_COLLECTION);
    template double integration_over_region_collections(
        environment::MultiModalBivariateGaussian,
        cubpackpp::REGION_COLLECTION);
    template double integration_over_region_collections(
        double(*)(double,double),
        cubpackpp::REGION_COLLECTION);

    template<typename FUNC, typename COORDS>
    double integrate_over_buffered_line(FUNC f, COORDS coords, double d)
    {
      std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), d);
      auto triangulated = geometry::triangulate_polygon(std::move(buffered));
      cubpackpp::REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);
      return integration_over_region_collections(f, rg);
    }

    template double integrate_over_buffered_line(function::Function, geometry::EigenCoords, double);
    template double integrate_over_buffered_line(function::Function, geometry::STLCoords, double);
    template double integrate_over_buffered_line(environment::MultiModalBivariateGaussian, geometry::EigenCoords, double);
    template double integrate_over_buffered_line(environment::MultiModalBivariateGaussian, geometry::STLCoords, double);
    template double integrate_over_buffered_line(double (*)(double, double), geometry::EigenCoords, double);
    template double integrate_over_buffered_line(double (*)(double, double), geometry::STLCoords, double);

    /// \brief Integrate multiple paths by buffering and merging before integrating.
    /// \tparam FUNC The callable type.
    /// \tparam COORDS The data strucutre of coordinates.
    /// \param f The callable holding the function to be integrated.
    /// \param coords_vec The std::vector of coordinates. Each object should be the coordinates for a distinc path.
    /// \param d The integration radius.
    /// \return The integration result.
    template<typename FUNC, typename COORDS>
    double integrate_over_buffered_lines(FUNC f, std::vector<COORDS> coords_vec, double d)
    {
      std::unique_ptr<Geometry> union_buffered_paths =
          geos::geom::GeometryFactory::getDefaultInstance()->createEmptyGeometry();
      for (auto coords : coords_vec)
      {
        std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
        auto ls = geometry::create_linestring(std::move(cs));
        std::unique_ptr<geos::geom::Geometry> buffered = geometry::buffer_linestring(std::move(ls), d);
        union_buffered_paths = union_buffered_paths->Union(buffered.get());
      }
      auto triangulated = geometry::triangulate_polygon(std::move(union_buffered_paths));
      cubpackpp::REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);
      return integration_over_region_collections(f, rg);
    }

    template double integrate_over_buffered_lines(function::Function, std::vector<geometry::STLCoords>, double);
    template double
    integrate_over_buffered_lines(environment::MultiModalBivariateGaussian, std::vector<geometry::EigenCoords>, double);
    template double
    integrate_over_buffered_lines(environment::MultiModalBivariateGaussian, std::vector<geometry::STLCoords>, double);
    template double integrate_over_buffered_lines(double (*)(double, double), std::vector<geometry::EigenCoords>, double);
    template double integrate_over_buffered_lines(double (*)(double, double), std::vector<geometry::STLCoords>, double);

    template<typename FUNC>
    double integrate_over_rect(FUNC f, double left, double right, double bottom, double top)
    {
      cubpackpp::REGION_COLLECTION rc;
      cubpackpp::Point A(left, bottom), B(left, top), C(right, bottom);
      cubpackpp::RECTANGLE rect(A, B, C);
      rc += rect;

      return integration_over_region_collections(f, rc);
    }
    template double integrate_over_rect(environment::MultiModalBivariateGaussian, double, double, double, double);
    template double integrate_over_rect(function::Function, double, double, double, double);
    template double integrate_over_rect(double (*)(double, double), double, double, double, double);

  }  // namespace integration
}  // namespace jpathgen