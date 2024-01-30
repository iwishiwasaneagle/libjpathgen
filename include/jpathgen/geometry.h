/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef JDRONES_GEOMETRY_H
#define JDRONES_GEOMETRY_H

#include <cubpackpp/cubpackpp.h>

#include "jpathgen/geos_compat.h"



#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Polygon.h>
#include <triangulate/tri/Tri.h>

#include <Eigen/Core>

namespace jpathgen
{
  namespace geometry
  {
    using CAS = geos::geom::CoordinateSequenceCompat;

    typedef Eigen::Matrix<double, Eigen::Dynamic, 2> EigenCoords;
    typedef std::vector<std::pair<double,double>> STLCoords;
    typedef std::vector<Coordinate> GeosCoords;

    extern geos::geom::GeometryFactory* _global_factory;

    template<typename COORDS>
    std::unique_ptr<CAS> coord_sequence_from_array(COORDS coords);

    std::unique_ptr<geos::geom::LineString> create_linestring(std::unique_ptr<CAS> cl);

    std::unique_ptr<Geometry> buffer_linestring(std::unique_ptr<geos::geom::LineString> ls, double d = 2.5);

    template<typename GEOM>
    std::unique_ptr<geos::geom::Geometry> triangulate_polygon(std::unique_ptr<GEOM> poly);

    void geos_to_cubpack(std::unique_ptr<geos::geom::Geometry> geoms, cubpackpp::REGION_COLLECTION& out_region);
  }  // namespace geometry
}  // namespace jpathgen

#endif  // JDRONES_GEOMETRY_H
