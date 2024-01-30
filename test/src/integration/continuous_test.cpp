/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <jpathgen/function.h>
#include <jpathgen/integration.h>
#include <math.h>

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

  EigenCoords build_coords(int N)
  {
    Eigen::MatrixXd angles = Eigen::MatrixXd::Random(N, 1);
    Eigen::MatrixX2d coords;
    coords.resize(N, 2);
    for (int i = 0; i < N; i++)
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
/*******************************************
 * TEST INTEGRATION OVER REGION COLLECTION *
 *******************************************/

TEST_CASE("Region collection is continuously integrated over", "[continuous, integration, region_collection]")
{
  double abs_err_req = GENERATE(0, 1e-10, 1e-6);
  double rel_err_req = GENERATE(0, 0.01, 0.05);
  cubpackpp::REGION_COLLECTION rc;
  double result = NAN;
  auto *continuous_args = new ContinuousArgs(abs_err_req = abs_err_req, rel_err_req = rel_err_req);

  SECTION("A unit square")
  {
    cubpackpp::Point A(0, 0), B(0, 1), C(1, 0);
    cubpackpp::RECTANGLE unit_square(A, B, C);
    rc += unit_square;

    result = continuous_integration_over_region_collections(constant_return_fn, rc, continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }

  SECTION("A rectangle")
  {
    cubpackpp::Point A(1, 1), B(1, 3), C(1.5, 1);
    cubpackpp::RECTANGLE rectangle(A, B, C);
    rc += rectangle;
    result = continuous_integration_over_region_collections(constant_return_fn, rc, continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }

  SECTION("A triangle")
  {
    cubpackpp::Point A(0, 0), B(0.5, 2), C(1, 0);
    cubpackpp::TRIANGLE triangle(A, B, C);
    rc += triangle;
    result = continuous_integration_over_region_collections(constant_return_fn, rc, continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }
  SECTION("Two triangles")
  {
    std::vector<STLCoords> triangles{ { { 1, 0 }, { 1.5, 1 }, { 2, 0 } }, { { 0, 0 }, { 0.5, 1 }, { 1, 0 } } };
    for (int i = 0; i < 2; i++)
    {
      cubpackpp::Point A(triangles[i][0].first, triangles[i][0].second), B(triangles[i][1].first, triangles[i][1].second),
          C(triangles[i][2].first, triangles[i][2].second);
      cubpackpp::TRIANGLE triangle(A, B, C);
      rc += triangle;
    }
    result = continuous_integration_over_region_collections(constant_return_fn, rc, continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }
  SECTION("Three triangles and a unit square")
  {
    std::vector<STLCoords> triangles{ { { -1, 0 }, { -0.5, 2 }, { 0, 0 } },
                                      { { 1, 0 }, { 1.5, 2 }, { 2, 0 } },
                                      { { 0, 0 }, { 0.5, 2 }, { 1, 0 } } };
    for (int i = 0; i < 3; i++)
    {
      cubpackpp::Point A(triangles[i][0].first, triangles[i][0].second), B(triangles[i][1].first, triangles[i][1].second),
          C(triangles[i][2].first, triangles[i][2].second);
      cubpackpp::TRIANGLE triangle(A, B, C);
      rc += triangle;
    }
    cubpackpp::Point A(0, 0), B(0, 1), C(1, 0);
    cubpackpp::RECTANGLE unit_square(A, B, C);
    rc += unit_square;

    result = continuous_integration_over_region_collections(constant_return_fn, rc, continuous_args);
    REQUIRE_THAT(4.0, WithinRel(result));
  }
}

/*********************************
 * TEST INTEGRATION OVER POLYGON *
 *********************************/

TEST_CASE("Polygon is continuously integrated over", "[continuous, integration, polygon, geos]")
{
  auto geometry_factory = geos::geom::GeometryFactory::getDefaultInstance();

  double abs_err_req = GENERATE(0, 1e-10, 1e-6);
  double rel_err_req = GENERATE(0, 0.01, 0.05);

  double result = NAN;
  auto *continuous_args = new ContinuousArgs(abs_err_req = abs_err_req, rel_err_req = rel_err_req);

  SECTION("A unit square")
  {
    STLCoords corners{ { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } };
    std::unique_ptr<geos::geom::CoordinateSequence> coordinate_sequence =
        jpathgen::geometry::coord_sequence_from_array(corners);
    std::unique_ptr<geos::geom::LinearRing> linear_ring = geometry_factory->createLinearRing(std::move(coordinate_sequence));
    std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));
    result = continuous_integration_over_polygon(constant_return_fn, std::move(polygon), continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }

  SECTION("A rectangle")
  {
    STLCoords corners{ { 0, 0 }, { 0, 0.5 }, { 2, 0.5 }, { 2, 0 }, { 0, 0 } };

    std::unique_ptr<geos::geom::CoordinateSequence> coordinate_sequence =
        jpathgen::geometry::coord_sequence_from_array(corners);
    std::unique_ptr<geos::geom::LinearRing> linear_ring = geometry_factory->createLinearRing(std::move(coordinate_sequence));
    std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));

    result = continuous_integration_over_polygon(constant_return_fn, std::move(polygon), continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }
  SECTION("Three triangles and a unit square")
  {
    std::vector<std::unique_ptr<geos::geom::Polygon>> polygons;
    polygons.reserve(4);
    std::vector<STLCoords> triangles{ { { -1, 0 }, { -0.5, 2 }, { 0, 0 }, { -1, 0 } },
                                      { { 1, 0 }, { 1.5, 2 }, { 2, 0 }, { 1, 0 } },
                                      { { 0, 0 }, { 0.5, 2 }, { 1, 0 }, { 0, 0 } } };
    for (int i = 0; i < 3; i++)
    {
      std::unique_ptr<geos::geom::CoordinateSequence> coordinate_sequence =
          jpathgen::geometry::coord_sequence_from_array(triangles[i]);
      std::unique_ptr<geos::geom::LinearRing> linear_ring =
          geometry_factory->createLinearRing(std::move(coordinate_sequence));
      std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));
      polygons.push_back(std::move(polygon));
    }
    std::vector<std::pair<double, double>> corners{ { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } };
    std::unique_ptr<geos::geom::CoordinateSequence> coordinate_sequence = coord_sequence_from_array(corners);
    std::unique_ptr<geos::geom::LinearRing> linear_ring = geometry_factory->createLinearRing(std::move(coordinate_sequence));
    std::unique_ptr<geos::geom::Polygon> polygon = geometry_factory->createPolygon(std::move(linear_ring));
    polygons.push_back(std::move(polygon));

    std::unique_ptr<geos::geom::MultiPolygon> multi_polygon = geometry_factory->createMultiPolygon(std::move(polygons));
    result = continuous_integration_over_polygon(constant_return_fn, std::move(multi_polygon), continuous_args);

    REQUIRE_THAT(4.0, WithinRel(result));
  }
  SECTION("A rectangle as a vector of coords")
  {
    STLCoords corners{ { 0, 0 }, { 0, 0.5 }, { 2, 0.5 }, { 2, 0 }, { 0, 0 } };
    result = continuous_integration_over_polygon(constant_return_fn, corners, continuous_args);
    REQUIRE_THAT(1.0, WithinRel(result));
  }
}

/******************************
 * TEST INTEGRATION OVER PATH *
 ******************************/

TEST_CASE("Buffered path is continuously integrated over", "[continuous, integration, path, geos]")
{
  int n_wps = GENERATE(2, 5, 10);

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

    REQUIRE_THAT(result, WithinRel(buffered_path->getArea(), 0.001));
  }
}

/*******************************
 * TEST INTEGRATION OVER PATHS *
 *******************************/

TEST_CASE("Buffered paths are continuously integrated over", "[continuous, integration, paths, geos]")
{
  int n_paths = GENERATE(2, 5, 10);
  double abs_err_req = GENERATE(0, 1e-6);
  double rel_err_req = GENERATE(0, 0.05);
  double buffer_radius_m = GENERATE(0.1, 1.5);

  double result = NAN;
  auto *continuous_args = new ContinuousArgs(buffer_radius_m, abs_err_req, rel_err_req);

  DYNAMIC_SECTION("EigenCoords with " << n_paths << " paths")
  {
    EigenCoords path;
    std::vector<EigenCoords> paths{};

    for (int i = 0; i < n_paths; i++)
    {
      path = build_coords(5);
      paths.push_back(path);
    }

    std::unique_ptr<geos::geom::CoordinateSequenceCompat> cs = coord_sequence_from_array(path);
    auto ls = create_linestring(std::move(cs));
    auto buffered_path = buffer_linestring(std::move(ls), continuous_args->get_buffer_radius_m());
    result = continuous_integration_over_path(constant_return_fn, path, continuous_args);
    REQUIRE_THAT(result, WithinRel(buffered_path->getArea(), 0.001));
  }

  DYNAMIC_SECTION("STLCoords with " << n_paths << " paths")
  {
    STLCoords path;
    std::vector<STLCoords> paths{};

    for (int i = 0; i < n_paths; i++)
    {
      path = eigen_to_stl_coords(build_coords(5));
      paths.push_back(path);
    }

    std::unique_ptr<geos::geom::CoordinateSequenceCompat> cs = coord_sequence_from_array(path);
    auto ls = create_linestring(std::move(cs));
    auto buffered_path = buffer_linestring(std::move(ls), continuous_args->get_buffer_radius_m());
    result = continuous_integration_over_path(constant_return_fn, path, continuous_args);
    REQUIRE_THAT(result, WithinRel(buffered_path->getArea(), 0.001));
  }
}
