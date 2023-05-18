// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/geometry.h"

#include <geos/geom/Coordinate.h>
#include <geos/triangulate/polygon/ConstrainedDelaunayTriangulator.h>

#include "jpathgen/error.h"

using geos::geom::Coordinate;
using geos::triangulate::polygon::ConstrainedDelaunayTriangulator;

namespace jpathgen
{
  namespace geometry
  {
    GeometryFactory* _global_factory;

    std::unique_ptr<CAS> coord_sequence_from_array(Eigen::Matrix<double, 2, Eigen::Dynamic> coords)
    {
      Error(coords.size() == 0, "Coordinate sequence is empty.");
      auto cas = std::make_unique<CAS>();

      double xi, yi;
      for (Eigen::Index i = 0; i < coords.rows(); i++)
      {
        Eigen::Vector2d coord = coords.row(i);
        xi = coord(0);
        yi = coord(1);
        cas->add(Coordinate(xi, yi));
      }
      return cas;
    }
    std::unique_ptr<CAS> coord_sequence_from_array(std::vector<std::pair<double, double>> coords)
    {
      Error(coords.size() == 0, "Coordinate sequence is empty.");
      auto cas = std::make_unique<CAS>();

      double xi, yi;
      for (std::pair<double, double> coord : coords)
      {
        xi = std::get<0>(coord);
        yi = std::get<1>(coord);
        cas->add(Coordinate(xi, yi));
      }

      return cas;
    }
    std::unique_ptr<CAS> coord_sequence_from_array(std::vector<Coordinate> coords)
    {
      Error(coords.size() == 0, "Coordinate sequence is empty.");

      auto cas = std::make_unique<CAS>();

      for (Coordinate coord : coords)
      {
        cas->add(coord);
      }

      return cas;
    }

    std::unique_ptr<LineString> create_linestring(std::unique_ptr<CAS> cl)
    {
      return _global_factory->createLineString(std::move(cl));
    }

    std::unique_ptr<Geometry> buffer_linestring(std::unique_ptr<LineString> ls, double d)
    {
      return ls->buffer(d);
    }
    std::unique_ptr<Geometry> triangulate_polygon(std::unique_ptr<Geometry> poly)
    {
      return ConstrainedDelaunayTriangulator::triangulate(poly.get());
    }

    void geos_to_cubpack(std::unique_ptr<Geometry> geoms, REGION_COLLECTION& out_region)
    {
      for (int i = 0; i < geoms->getNumGeometries(); i++)
      {
        auto coords = geoms->getGeometryN(i)->getCoordinates();

        if (coords->getSize() != 4 || !coords->isRing())
        {
          std::ostringstream ss;
          ss << "Expected a triangle. Got a coordinate sequence of length " << coords->getSize() - 1 << std::endl
             << "\t" << coords->toString() << std::endl
             << "\t isRing = " << coords->isRing() << std::endl;
          throw std::runtime_error(ss.str());
        }

        auto a = coords->getAt(0), b = coords->getAt(1), c = coords->getAt(2);

        Pt a_cp(a.x, a.y);
        Pt b_cp(b.x, b.y);
        Pt c_cp(c.x, c.y);

        TRIANGLE tr(a_cp, b_cp, c_cp);

        out_region += tr;
      }
    }
  }  // namespace geometry

}  // namespace jpathgen
