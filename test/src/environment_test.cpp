// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <jpathgen/environment.h>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <map>

using namespace jpathgen::environment;
using Catch::Matchers::WithinRel;

enum METHOD{STL, EIGEN};

std::map<int, std::string> METHOD2STRING = {
  {METHOD::STL, "STL"},
  {METHOD::EIGEN, "EIGEN"},
};

MultiModalBivariateGaussian create_unit_mmbg_using_stl(int n_modes){
  STLMUS mus;
  STLCOVS  covs;
  for (int i=0; i<n_modes;i++){
    mus.insert(mus.end(),1,{0,0});
    covs.insert(covs.end(), 1, {1,0});
    covs.insert(covs.end(), 1, {0,1});
  }
  MultiModalBivariateGaussian mmbg(mus, covs);
  return mmbg;
}

MultiModalBivariateGaussian create_unit_mmbg_using_eigen(int n_modes){

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

MultiModalBivariateGaussian create_unit_mmbg(int n_modes, METHOD method){
  switch (method)
  {
    case METHOD::STL:
      return create_unit_mmbg_using_stl(n_modes);
    case METHOD::EIGEN:
      return create_unit_mmbg_using_eigen(n_modes);
  }
}


SCENARIO("MVBivarGaussians can be instantiated", "[MVBG]")
{
  int N = GENERATE(1,2,5,10);
  METHOD method = GENERATE(METHOD::STL, METHOD::EIGEN);
  MultiModalBivariateGaussian mmbg = create_unit_mmbg(N, method);
  DYNAMIC_SECTION("With " << METHOD2STRING[method])
  {
    THEN("The length is N")
    {
      REQUIRE(mmbg.length() == N);
    }
    THEN("The value at (0,0) is 1/ ( 2 PI )")
    {
      REQUIRE_THAT(mmbg(0, 0), WithinRel(1 / (2 * M_PI)));
    }
    THEN("It is at a maxima at (0,0)")
    {
      double const exp = 1 / (2 * M_PI);
      for (double xi = -1; xi <= 1; xi += 0.5)
      {
        for (double yi = -1; yi <= 1; yi += 0.5)
        {
          if (xi == 0 && yi == 0)
          {
            continue;
          }
          CHECK(mmbg(xi, yi) < exp);
        }
      }
    }
  }
}

SCENARIO("MVBivarGaussians throws expected exceptions", "[MVBG]"){
  GIVEN("Incompatible mus and covs shapes are passed"){
    WHEN("The number of covs is lower than the number of means"){
      MUS mus = Eigen::Matrix<double, 4, 2>::Zero();
      COVS covs = Eigen::Matrix<double,4,2>::Zero();
      THEN("An exception is thrown"){
        REQUIRE_THROWS(MultiModalBivariateGaussian(mus, covs));
      }
    }
    WHEN("The number of covs is higher than the number of means"){
      MUS mus = Eigen::Matrix<double, 2, 2>::Zero();
      COVS covs = Eigen::Matrix<double,8,2>();
      THEN("An exception is thrown"){
        REQUIRE_THROWS(MultiModalBivariateGaussian(mus, covs));
      }
    }
  }
}

