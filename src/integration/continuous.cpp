/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Coordinate.h>
#include <geos/operation/union/UnaryUnionOp.h>
#include <geos/triangulate/tri/Tri.h>

#include <functional>
#include <utility>

#include "jpathgen/environment.h"
#include "jpathgen/function.h"
#include "jpathgen/geometry.h"
#include "jpathgen/geos_compat.h"
#include "jpathgen/integration.h"

using namespace geos::geom;
using namespace geos::triangulate::tri;

namespace jpathgen
{
  namespace integration
  {
    /*************************************************
     * CONTINUOUS INTEGRATION OVER REGION COLLECTION *
     *************************************************/
    template<typename FUNC>
    double continuous_integration_over_region_collections(FUNC f, cubpackpp::REGION_COLLECTION rc, ContinuousArgs* args)
    {
      cubpackpp::Function fn_bound = [&f](const cubpackpp::Point& pt)
      {
        double x = pt.X(), y = pt.Y();
        return f(x, y);
      };
      return continuous_integration_over_region_collections(fn_bound, rc, args);
    }
    template<>
    double continuous_integration_over_region_collections(
        cubpackpp::Function fn,
        cubpackpp::REGION_COLLECTION rc,
        ContinuousArgs* args)
    {
      return cubpackpp::Integrate(fn, rc, args->get_abs_err_req(), args->get_rel_err_req());
    }
    template double
    continuous_integration_over_region_collections(function::Function, cubpackpp::REGION_COLLECTION, ContinuousArgs*);
    template double continuous_integration_over_region_collections(
        environment::MultiModalBivariateGaussian,
        cubpackpp::REGION_COLLECTION,
        ContinuousArgs*);
    template double continuous_integration_over_region_collections(
        double (*)(double, double),
        cubpackpp::REGION_COLLECTION,
        ContinuousArgs*);

    /***************************************
     * CONTINUOUS INTEGRATION OVER POLYGON *
     ***************************************/

    template<typename FUNC>
    double continuous_integration_over_polygon(FUNC f, std::unique_ptr<geos::geom::Geometry> polygon, ContinuousArgs* args)
    {
      auto triangulated = geometry::triangulate_polygon(std::move(polygon));
      cubpackpp::REGION_COLLECTION rg;
      geometry::geos_to_cubpack(std::move(triangulated), rg);
      return continuous_integration_over_region_collections(f, rg, args);
    };
    template double
    continuous_integration_over_polygon(function::Function, std::unique_ptr<geos::geom::Geometry>, ContinuousArgs*);
    template double continuous_integration_over_polygon(
        environment::MultiModalBivariateGaussian,
        std::unique_ptr<geos::geom::Geometry>,
        ContinuousArgs*);
    template double
    continuous_integration_over_polygon(double (*)(double, double), std::unique_ptr<geos::geom::Geometry>, ContinuousArgs*);

    template<typename FUNC>
    double continuous_integration_over_polygon(FUNC f, geometry::STLCoords polygon, ContinuousArgs* args)
    {
      const geos::geom::GeometryFactory* geometry_factory = geos::geom::GeometryFactory::getDefaultInstance();

      std::unique_ptr<geos::geom::CoordinateSequence> coordinate_sequence = geometry::coord_sequence_from_array(polygon);
      std::unique_ptr<geos::geom::LinearRing> linear_ring =
          geometry_factory->createLinearRing(std::move(coordinate_sequence));
      std::unique_ptr<geos::geom::Polygon> geom = geometry_factory->createPolygon(std::move(linear_ring));
      return continuous_integration_over_polygon(f, std::move(geom), args);
    };
    template double continuous_integration_over_polygon(function::Function, geometry::STLCoords polygon, ContinuousArgs*);
    template double continuous_integration_over_polygon(
        environment::MultiModalBivariateGaussian,
        geometry::STLCoords polygon,
        ContinuousArgs*);
    template double
    continuous_integration_over_polygon(double (*)(double, double), geometry::STLCoords polygon, ContinuousArgs*);

    /************************************
     * CONTINUOUS INTEGRATION OVER PATH *
     ************************************/

    template<typename FUNC, typename COORDS>
    double continuous_integration_over_path(FUNC f, COORDS coords, ContinuousArgs* args)
    {
      std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), args->get_buffer_radius_m());
      return continuous_integration_over_polygon(f, std::move(buffered), args);
    }
    template double continuous_integration_over_path(function::Function, geometry::EigenCoords, ContinuousArgs*);
    template double continuous_integration_over_path(function::Function, geometry::STLCoords, ContinuousArgs*);
    template double
    continuous_integration_over_path(environment::MultiModalBivariateGaussian, geometry::EigenCoords, ContinuousArgs*);
    template double
    continuous_integration_over_path(environment::MultiModalBivariateGaussian, geometry::STLCoords, ContinuousArgs*);
    template double continuous_integration_over_path(double (*)(double, double), geometry::EigenCoords, ContinuousArgs*);
    template double continuous_integration_over_path(double (*)(double, double), geometry::STLCoords, ContinuousArgs*);

    /*************************************
     * CONTINUOUS INTEGRATION OVER PATHS *
     *************************************/

    template<typename FUNC, typename COORDS>
    double continuous_integration_over_paths(FUNC f, std::vector<COORDS> coords_vec, ContinuousArgs* args)
    {
      std::unique_ptr<Geometry> union_buffered_paths =
          geos::geom::GeometryFactory::getDefaultInstance()->createEmptyGeometry();
      for (auto coords : coords_vec)
      {
        std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
        auto ls = geometry::create_linestring(std::move(cs));
        std::unique_ptr<geos::geom::Geometry> buffered =
            geometry::buffer_linestring(std::move(ls), args->get_buffer_radius_m());
        union_buffered_paths = union_buffered_paths->Union(buffered.get());
      }
      return continuous_integration_over_polygon(f, std::move(union_buffered_paths), args);
    }

    template double
    continuous_integration_over_paths(function::Function, std::vector<geometry::EigenCoords>, ContinuousArgs*);
    template double continuous_integration_over_paths(function::Function, std::vector<geometry::STLCoords>, ContinuousArgs*);
    template double continuous_integration_over_paths(
        environment::MultiModalBivariateGaussian,
        std::vector<geometry::EigenCoords>,
        ContinuousArgs*);
    template double continuous_integration_over_paths(
        environment::MultiModalBivariateGaussian,
        std::vector<geometry::STLCoords>,
        ContinuousArgs*);
    template double
    continuous_integration_over_paths(double (*)(double, double), std::vector<geometry::EigenCoords>, ContinuousArgs*);
    template double
    continuous_integration_over_paths(double (*)(double, double), std::vector<geometry::STLCoords>, ContinuousArgs*);

    /*****************************************
     * CONTINUOUS INTEGRATION OVER RECTANGLE *
     *****************************************/

    template<typename FUNC>
    double
    continuous_integration_over_rectangle(FUNC f, double left, double right, double bottom, double top, ContinuousArgs* args)
    {
      cubpackpp::REGION_COLLECTION rc;
      cubpackpp::Point A(left, bottom), B(left, top), C(right, bottom);
      cubpackpp::RECTANGLE rect(A, B, C);
      rc += rect;

      return continuous_integration_over_region_collections(f, rc, args);
    }
    template double continuous_integration_over_rectangle(
        environment::MultiModalBivariateGaussian,
        double,
        double,
        double,
        double,
        ContinuousArgs*);
    template double
    continuous_integration_over_rectangle(function::Function, double, double, double, double, ContinuousArgs*);
    template double
    continuous_integration_over_rectangle(double (*)(double, double), double, double, double, double, ContinuousArgs*);

  }  // namespace integration
}  // namespace jpathgen