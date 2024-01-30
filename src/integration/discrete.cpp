/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Coordinate.h>
#include <geos/operation/union/UnaryUnionOp.h>

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
    /*************************************
     * DISCRETE INTEGRATION OVER POLYGON *
     *************************************/
    template<typename FUNC>
    double discrete_integration_over_polygon(FUNC f, std::unique_ptr<geos::geom::Geometry> polygon, DiscreteArgs* args)
    {
      double sum = 0;
      int count = 0;
      for (auto xi : Eigen::VectorXd::LinSpaced(args->get_N(), args->get_minx(), args->get_maxx()))
      {
        for (auto yi : Eigen::VectorXd::LinSpaced(args->get_M(), args->get_miny(), args->get_maxy()))
        {
          std::unique_ptr<geos::geom::Point> pt =
              geos::geom::GeometryFactory::getDefaultInstance()->createPoint(geos::geom::Coordinate(xi, yi));
          if (polygon->contains(std::move(pt).get()))
          {
            sum += f(xi, yi);
            count++;
          }
        }
      }
      return sum * (args->get_maxx() - args->get_minx()) * (args->get_maxy() - args->get_miny()) / (args->get_N()* args->get_M());
    }
    template double
    discrete_integration_over_polygon(function::Function, std::unique_ptr<geos::geom::Geometry>, DiscreteArgs*);
    template double discrete_integration_over_polygon(
        environment::MultiModalBivariateGaussian,
        std::unique_ptr<geos::geom::Geometry>,
        DiscreteArgs*);
    template double
    discrete_integration_over_polygon(double (*)(double, double), std::unique_ptr<geos::geom::Geometry>, DiscreteArgs*);

    template<typename FUNC>
    double discrete_integration_over_polygon(FUNC f, geometry::STLCoords polygon, DiscreteArgs* args)
    {
      const geos::geom::GeometryFactory* geometry_factory = geos::geom::GeometryFactory::getDefaultInstance();
      std::unique_ptr<geos::geom::CoordinateSequence> coordinate_sequence = geometry::coord_sequence_from_array(polygon);
      std::unique_ptr<geos::geom::LinearRing> linear_ring =
          geometry_factory->createLinearRing(std::move(coordinate_sequence));
      std::unique_ptr<geos::geom::Polygon> geom = geometry_factory->createPolygon(std::move(linear_ring));
      return discrete_integration_over_polygon(f, std::move(geom), args);
    }
    template double discrete_integration_over_polygon(function::Function, geometry::STLCoords polygon, DiscreteArgs*);
    template double
    discrete_integration_over_polygon(environment::MultiModalBivariateGaussian, geometry::STLCoords polygon, DiscreteArgs*);
    template double
    discrete_integration_over_polygon(double (*)(double, double), geometry::STLCoords polygon, DiscreteArgs*);
    /***************************************
     * DISCRETE INTEGRATION OVER RECTANGLE *
     ***************************************/
    template<typename FUNC>
    double
    discrete_integration_over_rectangle(FUNC f, double left, double right, double bottom, double top, DiscreteArgs* args)
    {
      geometry::STLCoords coords{ { left, bottom }, { left, top }, { right, top }, { right, bottom }, { left, bottom } };
      return discrete_integration_over_polygon(f, coords, args);
    };
    template double discrete_integration_over_rectangle(
        environment::MultiModalBivariateGaussian,
        double,
        double,
        double,
        double,
        DiscreteArgs*);
    template double discrete_integration_over_rectangle(function::Function, double, double, double, double, DiscreteArgs*);
    template double
    discrete_integration_over_rectangle(double (*)(double, double), double, double, double, double, DiscreteArgs*);

    /**********************************
     * DISCRETE INTEGRATION OVER PATH *
     **********************************/
    template<typename FUNC, typename COORDS>
    double discrete_integration_over_path(FUNC f, COORDS coords, DiscreteArgs* args)
    {
      std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
      auto ls = geometry::create_linestring(std::move(cs));
      auto buffered = geometry::buffer_linestring(std::move(ls), args->get_buffer_radius_m());
      return discrete_integration_over_polygon(f, std::move(buffered), args);
    }
    template double discrete_integration_over_path(function::Function, geometry::EigenCoords, DiscreteArgs*);
    template double discrete_integration_over_path(function::Function, geometry::STLCoords, DiscreteArgs*);
    template double
    discrete_integration_over_path(environment::MultiModalBivariateGaussian, geometry::EigenCoords, DiscreteArgs*);
    template double
    discrete_integration_over_path(environment::MultiModalBivariateGaussian, geometry::STLCoords, DiscreteArgs*);
    template double discrete_integration_over_path(double (*)(double, double), geometry::EigenCoords, DiscreteArgs*);
    template double discrete_integration_over_path(double (*)(double, double), geometry::STLCoords, DiscreteArgs*);

    /*************************************
     * CONTINUOUS INTEGRATION OVER PATHS *
     *************************************/

    template<typename FUNC, typename COORDS>
    double discrete_integration_over_paths(FUNC f, std::vector<COORDS> coords_vec, DiscreteArgs* args)
    {
      std::unique_ptr<Geometry> union_buffered_paths =
          geos::geom::GeometryFactory::getDefaultInstance()->createEmptyGeometry();
      for (auto coords : coords_vec)
      {
        std::unique_ptr<CoordinateSequenceCompat> cs = geometry::coord_sequence_from_array(coords);
        auto ls = geometry::create_linestring(std::move(cs));
        std::unique_ptr<geos::geom::Geometry> buffered = geometry::buffer_linestring(std::move(ls), args->get_buffer_radius_m());
        union_buffered_paths = union_buffered_paths->Union(buffered.get());
      }
      return discrete_integration_over_polygon(f, std::move(union_buffered_paths), args);
    }

    template double discrete_integration_over_paths(function::Function, std::vector<geometry::EigenCoords>, DiscreteArgs*);
    template double discrete_integration_over_paths(function::Function, std::vector<geometry::STLCoords>, DiscreteArgs*);
    template double discrete_integration_over_paths(
        environment::MultiModalBivariateGaussian,
        std::vector<geometry::EigenCoords>,
        DiscreteArgs*);
    template double discrete_integration_over_paths(
        environment::MultiModalBivariateGaussian,
        std::vector<geometry::STLCoords>,
        DiscreteArgs*);
    template double
    discrete_integration_over_paths(double (*)(double, double), std::vector<geometry::EigenCoords>, DiscreteArgs*);
    template double
    discrete_integration_over_paths(double (*)(double, double), std::vector<geometry::STLCoords>, DiscreteArgs*);
  }  // namespace integration
}  // namespace jpathgen