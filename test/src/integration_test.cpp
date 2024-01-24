// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only
#include <jpathgen/environment.h>
#include <jpathgen/function.h>
#include <jpathgen/integration.h>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <functional>
using namespace std::placeholders;  // for _1, _2, _3...

using namespace jpathgen::integration;
using namespace jpathgen::function;
using namespace jpathgen::environment;
using namespace jpathgen::geometry;
using Catch::Matchers::WithinRel;

double fn1(double a, double b)
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

SCENARIO("The integration function is called with the various allowed types", "[integration]")
{
  SECTION("A function::Function")
  {
    auto fn = fn1;
    WHEN("And a Eigen::Matrix of coords is passed")
    {
      EigenCoords coords = Eigen::Matrix<double, 10, 2>::Random();
      THEN("A double is returned without error")
      {
        REQUIRE(integrate_over_buffered_line(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a list of coords in the format std::vector<Eigen::Matrix> is passed")
    {
      EigenCoords a = Eigen::Matrix<double, 10, 2>::Random();
      EigenCoords b = Eigen::Matrix<double, 3, 2>::Random();
      std::vector<EigenCoords> coords;
      coords.push_back(a);
      coords.push_back(b);
      THEN("A double is returned without error")
      {
        REQUIRE(integrate_over_buffered_lines(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a vector of coords is passed")
    {
      STLCoords coords = { { 0, 0 }, { 0, 1 }, { 10, 10 } };
      THEN("A double is returned without error")
      {
        REQUIRE(integrate_over_buffered_line(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a list of coords in the format std::vectorSTLCoords> is passed")
    {
      STLCoords a = { { 0, 0 }, { 0, 1 }, { 10, 10 } };
      STLCoords b = { { 0, 0 }, { 10, 10 } };
      std::vector<STLCoords> coords;
      coords.push_back(a);
      coords.push_back(b);
      THEN("A double is returned without error")
      {
        REQUIRE(integrate_over_buffered_lines(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a rectangle is passed")
    {
      double act = integrate_over_rect(fn, 0, 10, 0, 10);
      THEN("The value is as expected")
      {
        REQUIRE(act == 10 * 10);
      }
    }
  }
  SECTION("A MultiModalBivariateGaussian")
  {
    int n_modes = GENERATE(1, 10, 100);
    MultiModalBivariateGaussian fn = generate_mmbg(n_modes);
    WHEN("And a Eigen::Matrix of coords is passed")
    {
      int n_coords = GENERATE(2, 50);
      EigenCoords coords = Eigen::Matrix<double, -1, 2>::Random(n_coords, 2);
      THEN("A double is returned without error")
      {
        REQUIRE(integrate_over_buffered_line(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a vector of coords is passed")
    {
      STLCoords coords = { { 0, 0 }, { 0, 1 }, { 10, 10 } };
      THEN("A double is returned without error")
      {
        REQUIRE(integrate_over_buffered_line(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a rectangle is passed")
    {
      double act = integrate_over_rect(fn, 0, 10, 0, 10);
      THEN("The value is as expected")
      {
        REQUIRE_THAT(act, WithinRel(0.25, 1e-6));
      }
    }
  }
}

SCENARIO("A step function with increasingly small plateau is evaluated", "[integration]")
{
  WHEN("A width is set to something small")
  {
    double width = GENERATE(range(0.1, 1.0, 0.1));
    Function fn = [width](double a, double b) { return fn_step(a, b, width); };
    THEN("Then the double integral function adapts")
    {
      const double act = integrate_over_rect(fn, -1, 1, -1, 1);
      const double exp = width * width;
      double rel_error = abs((act - exp) / exp);
      REQUIRE(rel_error < 0.005);  // Within 0.5%
    }
  }
}
