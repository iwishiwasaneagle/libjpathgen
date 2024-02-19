/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef JDRONES_INTEGRATION_H
#define JDRONES_INTEGRATION_H

#include <cubpackpp/cubpackpp.h>
#include <geos/geom/Polygon.h>

#include <memory>

#include "jpathgen/geometry.h"

namespace jpathgen
{
  namespace integration
  {
    class Args
    {
     protected:
      const double _buffer_radius_m;

     public:
      [[nodiscard]] double get_buffer_radius_m() const
      {
        return _buffer_radius_m;
      }
      explicit Args(double buffer_radius_m = 2.5) : _buffer_radius_m(buffer_radius_m){};
    };

    class ContinuousArgs : public Args
    {
     protected:
      const double _abs_err_req;
      const double _rel_err_req;

     public:
      [[nodiscard]] double get_abs_err_req() const
      {
        return _abs_err_req;
      }
      [[nodiscard]] double get_rel_err_req() const
      {
        return _rel_err_req;
      }

      explicit ContinuousArgs(double buffer_radius_m, double abs_err_req = 0, double rel_err_req = 0.05)
          : Args(buffer_radius_m),
            _abs_err_req(abs_err_req),
            _rel_err_req(rel_err_req){};
    };

    class DiscreteArgs : public Args
    {
     protected:
      const int _N, _M;
      const double _minx, _maxx, _miny, _maxy;

     public:
      [[nodiscard]] int get_N() const
      {
        return _N;
      }
      [[nodiscard]] int get_M() const
      {
        return _M;
      }
      [[nodiscard]] double get_minx() const
      {
        return _minx;
      }
      [[nodiscard]] double get_maxx() const
      {
        return _maxx;
      }
      [[nodiscard]] double get_miny() const
      {
        return _miny;
      }
      [[nodiscard]] double get_maxy() const
      {
        return _maxy;
      }
      explicit DiscreteArgs(double buffer_radius_m, int N, int M, double minx, double maxx, double miny, double maxy)
          : Args(buffer_radius_m),
            _N(N),
            _M(M),
            _minx(minx),
            _maxx(maxx),
            _miny(miny),
            _maxy(maxy){};
      explicit DiscreteArgs(
          double buffer_radius_m,
          int N,
          int M,
          geos::geom::Envelope envelope,
          double rel_offset = 0.0,
          double abs_offset = 0.0)
          : Args(buffer_radius_m),
            _N(N),
            _M(M),
            _minx(envelope.getMinX() * (1 - rel_offset) - abs_offset),
            _maxx(envelope.getMaxX() * (1 + rel_offset) + abs_offset),
            _miny(envelope.getMinY() * (1 - rel_offset) - abs_offset),
            _maxy(envelope.getMaxY() * (1 + rel_offset) + abs_offset){};
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

    template<typename FUNC, typename COORDS>
    double discrete_integration_over_path(FUNC f, COORDS coords, DiscreteArgs* args);
    template<typename FUNC, typename COORDS>
    double discrete_integration_over_paths(FUNC f, std::vector<COORDS> coords, DiscreteArgs* args);
    template<typename FUNC>
    double
    discrete_integration_over_rectangle(FUNC f, double left, double right, double bottom, double top, DiscreteArgs* args);
    template<typename FUNC>
    double discrete_integration_over_polygon(FUNC f, std::unique_ptr<geos::geom::Geometry> polygon, DiscreteArgs* args);
    template<typename FUNC>
    double discrete_integration_over_polygon(FUNC f, geometry::STLCoords polygon, DiscreteArgs* args);

  }  // namespace integration
}  // namespace jpathgen
#endif  // JDRONES_INTEGRATION_H
