// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <jpathgen/integration.h>
#include <jpathgen/environment.h>
#include <jpathgen/function.h>
#include <jpathgen/geometry.h>

#include <Eigen/Core>

using namespace jpathgen::integration;
using namespace jpathgen::function;
using namespace jpathgen::environment;
using namespace jpathgen::geometry;

double fn1(double a, double b){
  return 1;
}

SCENARIO("The integration function is called with the various allowed types", "[integration]"){
  GIVEN("A function::Function"){
    WHEN("And a Eigen::Matrix of coords is passed"){
      EigenCoords coords = Eigen::Matrix<double, 10,2>::Random();
      THEN("A double is returned without error"){
        REQUIRE(integrate_over_buffered_line(fn1, coords) > -INFINITY);
      }
    }
    WHEN("And a vector of coords is passed"){
      STLCoords coords = {{0,0},{0,1},{10,10}};
      THEN("A double is returned without error"){
        REQUIRE(integrate_over_buffered_line(fn1, coords) > -INFINITY);
      }
    }
  }
  GIVEN("A MultiModalBivariateGaussian"){
    MU mu = MU::Zero();
    COV cov = COV::Identity();
    MultiModalBivariateGaussian mmbg(mu, cov);

    WHEN("And a Eigen::Matrix of coords is passed"){
      EigenCoords coords = Eigen::Matrix<double, 10,2>::Random();
      THEN("A double is returned without error"){
        REQUIRE(integrate_over_buffered_line(mmbg, coords) > -INFINITY);
      }
    }
    WHEN("And a vector of coords is passed"){
      STLCoords coords = {{0,0},{0,1},{10,10}};
      THEN("A double is returned without error"){
        REQUIRE(integrate_over_buffered_line(mmbg, coords) > -INFINITY);
      }
    }
  }
}

