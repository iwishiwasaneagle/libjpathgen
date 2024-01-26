/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <geos/geom/Coordinate.h>

#include "jpathgen/environment.h"
#include "jpathgen/error.h"
#include "jpathgen/geometry.h"
namespace jpathgen
{
  namespace geometry
  {
    template<>
    std::unique_ptr<CAS> coord_sequence_from_array(EigenCoords coords)
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

    template<>
    std::unique_ptr<CAS> coord_sequence_from_array(STLCoords coords)
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

    template<>
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
  }  // namespace geometry
}  // namespace jpathgen