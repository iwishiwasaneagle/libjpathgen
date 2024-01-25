// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/geometry.h"

#include <geos/geom/Coordinate.h>
#include <geos/triangulate/polygon/ConstrainedDelaunayTriangulator.h>

namespace jpathgen
{
  namespace geometry
  {
    using geos::geom::Coordinate;
    using geos::geom::LineString;
    using geos::triangulate::polygon::ConstrainedDelaunayTriangulator;
    using Pt = cubpackpp::Point;
    using cubpackpp::REGION_COLLECTION;
    using cubpackpp::TRIANGLE;

    GeometryFactory* _global_factory;

    std::unique_ptr<LineString> create_linestring(std::unique_ptr<CAS> cl)
    {
      return _global_factory->createLineString(std::move(cl));
    }

    std::unique_ptr<Geometry> buffer_linestring(std::unique_ptr<LineString> ls, double d)
    {
      return ls->buffer(d);
    }

    template<typename GEOM>
    std::unique_ptr<Geometry> triangulate_polygon(std::unique_ptr<GEOM> poly)
    {
      return ConstrainedDelaunayTriangulator::triangulate(poly.get());
    }
    template std::unique_ptr<Geometry> triangulate_polygon(std::unique_ptr<Geometry>);
    template std::unique_ptr<Geometry> triangulate_polygon(std::unique_ptr<Polygon>);
    template std::unique_ptr<Geometry> triangulate_polygon(std::unique_ptr<geos::geom::MultiPolygon>);

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
