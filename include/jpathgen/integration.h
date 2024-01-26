/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef JDRONES_INTEGRATION_H
#define JDRONES_INTEGRATION_H

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Polygon.h>

#include <Eigen/Core>
#include <memory>

#include "jpathgen/environment.h"
#include "jpathgen/function.h"
#include "jpathgen/geometry.h"
#include "jpathgen/geos_compat.h"

namespace jpathgen
{
  namespace integration
  {
    class Args
    {
     public:
      const double _buffer_radius_m;
      explicit Args(double buffer_radius_m = 2.5) : _buffer_radius_m(buffer_radius_m){};
    };

    class ContinuousArgs : public Args
    {
     public:
      const double _abs_err_req;
      const double _rel_err_req;
      explicit ContinuousArgs(double buffer_radius_m, double abs_err_req = 0, double rel_err_req = 0.05)
          : Args(buffer_radius_m),
            _abs_err_req(abs_err_req),
            _rel_err_req(rel_err_req){};
    };

    class DiscreteArgs : public Args
    {
    };

    template<typename FUNC, typename COORDS>
    double continuous_integration_over_path(FUNC f, COORDS coords, ContinuousArgs* args);
    template<typename FUNC, typename COORDS>
    double continuous_integration_over_paths(FUNC f, std::vector<COORDS> coords, ContinuousArgs* args);
    template<typename FUNC>
    double continuous_integration_over_rectangle(
        FUNC f,
        double left,
        double right,
        double bottom,
        double top,
        ContinuousArgs* args);
    template<typename FUNC>
    double continuous_integration_over_polygon(FUNC f, std::unique_ptr<geos::geom::Geometry> polygon, ContinuousArgs* args);
    template<typename FUNC>
    double continuous_integration_over_polygon(FUNC f, geometry::STLCoords polygon, ContinuousArgs* args);
    template<typename FUNC>
    double continuous_integration_over_region_collections(FUNC f, cubpackpp::REGION_COLLECTION rc, ContinuousArgs* args);

    // template<typename IMAGE, typename COORDS>
    // double discrete_integration_over_path(IMAGE img, COORDS coords, DiscreteArgs* args);
    // template<typename IMAGE, typename COORDS>
    // double discrete_integration_over_paths(IMAGE img, std::vector<COORDS> coords, DiscreteArgs* args);
    // template<typename IMAGE>
    // double discrete_integration_over_polygon(IMAGE img, std::unique_ptr<geos::geom::Geometry> polygon, DiscreteArgs*
    // args);

  }  // namespace integration
}  // namespace jpathgen
#endif  // JDRONES_INTEGRATION_H
