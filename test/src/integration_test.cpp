// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only
#include <jpathgen/environment.h>
#include <jpathgen/function.h>
#include <jpathgen/integration.h>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <map>

using namespace jpathgen::integration;
using namespace jpathgen::function;
using namespace jpathgen::environment;
using namespace jpathgen::geometry;
using Catch::Matchers::WithinRel;

double fn1(double a, double b){
  return 1;
}

MultiModalBivariateGaussian generate_mmbg(int n_modes=1){
  MUS mus = Eigen::Matrix<double, -1, 2>::Zero(n_modes,2);
  COV cov = COV::Identity();
  COVS covs = Eigen::Matrix<double, -1, 2>::Zero(n_modes*2, 2);

  for (int i = 0; i < n_modes; ++i)
  {
    covs.block<2,2>(i*2, 0) = cov;
  }
  MultiModalBivariateGaussian mmbg(mus, covs);
  return mmbg;
}

enum CALLABLE{FUNCTION, MMBG};
std::map<int, std::string> CALLABLE2STRING = {
  {CALLABLE::FUNCTION, "function::Function"},
  {CALLABLE::MMBG, "MultiModalBivariateGaussian"},
};

SCENARIO("The integration function is called with the various allowed types", "[integration]"){
  int n_modes = GENERATE(1, 5, 10);
  CALLABLE method = GENERATE(CALLABLE::FUNCTION, CALLABLE::MMBG);
  Function fn;
  switch (method)
  {
    case CALLABLE::FUNCTION:
      fn = fn1;
      break;
    case CALLABLE::MMBG:
      fn = generate_mmbg(n_modes);
      break;
  }

  DYNAMIC_SECTION("A " << CALLABLE2STRING[method]){
    WHEN("And a Eigen::Matrix of coords is passed"){
      EigenCoords coords = Eigen::Matrix<double, 10,2>::Random();
      THEN("A double is returned without error"){
        REQUIRE(integrate_over_buffered_line(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a vector of coords is passed"){
      STLCoords coords = {{0,0},{0,1},{10,10}};
      THEN("A double is returned without error"){
        REQUIRE(integrate_over_buffered_line(fn, coords) > -INFINITY);
      }
    }
    WHEN("And a rectangle is passed"){
      double act = integrate_over_rect(fn, 0,10,0,10);
      THEN("A non-zero value is returned"){
        REQUIRE(act > 0 );
      }
      THEN("The value is as expected"){
        switch (method)
        {
          case CALLABLE::FUNCTION:
            REQUIRE(act == 10*10);
            break;
          case CALLABLE::MMBG:
            REQUIRE_THAT(0.25, WithinRel(act, 1e-6));
            break;
        }
      }
    }
  }
}

