/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <jpathgen/function.h>
#include <jpathgen/integration.h>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace jpathgen::integration;
using namespace jpathgen::function;
using namespace jpathgen::environment;
using namespace jpathgen::geometry;
using Catch::Matchers::WithinRel;

#define REL_ACCEPTABLE_ERROR 0.01
#define N                    1000
#define M                    1000

namespace
{
  double constant_return_fn(double a, double b)
  {
    return 1;
  }

  MultiModalBivariateGaussian generate_mmbg(int n_modes = 1)
  {
    MUS mus = Eigen::Matrix<double, -1, 2>::Zero(n_modes, 2);
    COV cov = COV::Identity();
    COVS covs = Eigen::Matrix<double, -1, 2>::Zero(n_modes * 2, 2);

    for (int i = 0; i < n_modes; ++i)
    {
      covs.block<2, 2>(i * 2, 0) = cov;
    }
    MultiModalBivariateGaussian mmbg(mus, covs);
    return mmbg;
  }

  double fn_step(double a, double b, double width = 1)
  {
    if (a >= -width / 2 && a <= width / 2 && b >= -width / 2 && b <= width / 2)
    {
      return 1;
    }
    return 0;
  }

  EigenCoords build_coords(int n)
  {
    Eigen::MatrixXd angles = Eigen::MatrixXd::Random(n, 1);
    Eigen::MatrixX2d coords;
    coords.resize(n, 2);
    for (int i = 0; i < n; i++)
    {
      auto sliced_angles = angles.block(0, 0, i, 1).array();
      coords(i, 0) = sliced_angles.cos().sum();
      coords(i, 1) = sliced_angles.sin().sum();
    }
    return coords;
  }
  STLCoords eigen_to_stl_coords(EigenCoords coords)
  {
    STLCoords stl_coords;
    for (int i = 0; i < coords.rows(); i++)
    {
      stl_coords.emplace_back(coords(i, 0), coords(i, 1));
    }
    return stl_coords;
  }
}  // namespace
/*********************************
 * TEST INTEGRATION OVER POLYGON *
 *********************************/

TEST_CASE("Polygon is integrated over", "[discrete, integration, polygon, geos]")
{
  auto geometry_factory = geos::geom::GeometryFactory::getDefaultInstance();

  double xoffset = GENERATE(0.0, 50.0, -50.0);
  double yoffset = xoffset;

  double result = NAN;

  SECTION("A unit square")
  {
    double corners[5][2]{ { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } };

    geos::geom::CoordinateSequence coordinate_sequence;
    for (double *corner : corners)
    {
      geos::geom::Coordinate coordinate(corner[0] + xoffset, corner[1] + yoffset);
      coordinate_sequence.add(coordinate);
    }
    std::unique_ptr<geos::geom::LinearRing> linear_ring = geometry_factory->createLinearRing(coordinate_sequence);
    std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));

    auto envelope = coordinate_sequence.getEnvelope();
    auto *discrete_args = new DiscreteArgs(
        2.5,
        N,
        M,
        envelope.getMinX() * 0.95,
        envelope.getMaxX() * 1.05,
        envelope.getMinY() * 0.95,
        envelope.getMaxY() * 1.05);
    result = discrete_integration_over_polygon(constant_return_fn, std::move(polygon), discrete_args);
    REQUIRE_THAT(1.0, WithinRel(result, REL_ACCEPTABLE_ERROR));
  }

  SECTION("A rectangle")
  {
    double corners[5][2]{ { 0, 0 }, { 0, 0.5 }, { 2, 0.5 }, { 2, 0 }, { 0, 0 } };

    geos::geom::CoordinateSequence coordinate_sequence;
    for (double *corner : corners)
    {
      geos::geom::Coordinate coordinate(corner[0] + xoffset, corner[1] + yoffset);
      coordinate_sequence.add(coordinate);
    }
    std::unique_ptr<geos::geom::LinearRing> linear_ring = geometry_factory->createLinearRing(coordinate_sequence);
    std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));

    auto *discrete_args = new DiscreteArgs(
        2.5,
        N,
        M,
        xoffset-1,
        xoffset+3,
        yoffset-1,
        yoffset+3);
    result = discrete_integration_over_polygon(constant_return_fn, std::move(polygon), discrete_args);
    REQUIRE_THAT(1.0, WithinRel(result, REL_ACCEPTABLE_ERROR));
  }
  SECTION("Three triangles")
  {
    std::vector<std::unique_ptr<geos::geom::Polygon>> polygons;
    polygons.reserve(4);
    double triangles[3][4][2]{ { { -1, 0 }, { -0.5, 2 }, { 0, 0 }, { -1, 0 } },
                               { { 1, 0 }, { 1.5, 2 }, { 2, 0 }, { 1, 0 } },
                               { { 0, 0 }, { 0.5, 2 }, { 1, 0 }, { 0, 0 } } };
    for (int i = 0; i < 3; i++)
    {
      geos::geom::CoordinateSequence coordinate_sequence;
      for (int j = 0; j < 4; j++)
      {
        geos::geom::Coordinate coordinate(triangles[i][j][0] + xoffset, triangles[i][j][1] + yoffset);
        coordinate_sequence.add(coordinate);
      }
      std::unique_ptr<geos::geom::LinearRing> linear_ring = geometry_factory->createLinearRing(coordinate_sequence);
      std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));
      polygons.push_back(std::move(polygon));
    }

    std::unique_ptr<geos::geom::MultiPolygon> multi_polygon = geometry_factory->createMultiPolygon(std::move(polygons));
    assert(multi_polygon->isValid());

    auto *discrete_args = new DiscreteArgs(2.5, N, M, -2 + xoffset, 3 + xoffset, -1 + yoffset, 3 + yoffset);
    result = discrete_integration_over_polygon(constant_return_fn, std::move(multi_polygon), discrete_args);

    REQUIRE_THAT(3.0, WithinRel(result, REL_ACCEPTABLE_ERROR));
  }
  SECTION("A rectangle as a vector of coords")
  {
    STLCoords corners {
    { 0 + xoffset, yoffset + 0 },
    { 0 + xoffset, yoffset + 0.5 },
    { 2 + xoffset, yoffset + 0.5 },
    { 2 + xoffset, yoffset + 0 },
    { 0 + xoffset, yoffset + 0 }};
    auto *discrete_args = new DiscreteArgs(2.5, N, M, -1 + xoffset, 3 + xoffset, -1 + yoffset, 1 + yoffset);
    result = discrete_integration_over_polygon(constant_return_fn, std::move(corners), discrete_args);
    REQUIRE_THAT(1.0, WithinRel(result, REL_ACCEPTABLE_ERROR));
  }
}

/******************************
 * TEST INTEGRATION OVER PATH *
 ******************************/

TEST_CASE("Buffered path is integrated over", "[discrete, integration, path, geos]")
{
  int n_wps = GENERATE(range(2, 10, 1));

  double abs_err_req = GENERATE(0, 1e-10, 1e-6);
  double rel_err_req = GENERATE(0, 0.01, 0.05);
  double buffer_radius_m = GENERATE(1.0, 2.5, 5.);

  double result = NAN;
  auto *continuous_args = new ContinuousArgs(buffer_radius_m, abs_err_req, rel_err_req);

  DYNAMIC_SECTION("A " << n_wps << "-waypoint path")
  {
    EigenCoords path = Eigen::Matrix<double, -1, 2>::Random(n_wps, 2);

    std::unique_ptr<geos::geom::CoordinateSequenceCompat> cs = coord_sequence_from_array(path);
    auto ls = create_linestring(std::move(cs));
    auto buffered_path = buffer_linestring(std::move(ls), continuous_args->get_buffer_radius_m());

    result = continuous_integration_over_path(constant_return_fn, path, continuous_args);

    REQUIRE_THAT(result, WithinRel(buffered_path->getArea(), REL_ACCEPTABLE_ERROR));
  }
}
