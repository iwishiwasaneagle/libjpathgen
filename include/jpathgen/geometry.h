// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#ifndef JDRONES_GEOMETRY_H
#define JDRONES_GEOMETRY_H

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Polygon.h>
#include <triangulate/tri/Tri.h>

#include <Eigen/Core>

using CAS = geos::geom::CoordinateArraySequence;
using cubpackpp::REGION_COLLECTION;
using cubpackpp::TRIANGLE;
using geos::geom::Geometry;
using geos::geom::GeometryFactory;
using geos::geom::LineString;
using geos::geom::MultiPolygon;
using geos::geom::Polygon;
using Pt = cubpackpp::Point;

namespace jpathgen
{
  namespace geometry
  {
    extern GeometryFactory* _global_factory;

    std::unique_ptr<CAS> coord_sequence_from_array(std::vector<std::pair<double, double>> coords);
    std::unique_ptr<CAS> coord_sequence_from_array(Eigen::Matrix<double, 2, Eigen::Dynamic> coords);
    std::unique_ptr<CAS> coord_sequence_from_array(std::vector<Coordinate> coords);
    std::unique_ptr<LineString> create_linestring(std::unique_ptr<CAS> cl);
    std::unique_ptr<Geometry> buffer_linestring(std::unique_ptr<LineString> ls, double d = 2.5);
    std::unique_ptr<Geometry> triangulate_polygon(std::unique_ptr<Geometry> poly);

    void geos_to_cubpack(std::unique_ptr<Geometry> geoms, REGION_COLLECTION& out_region);
  }  // namespace geometry
}  // namespace jpathgen

#endif  // JDRONES_GEOMETRY_H
