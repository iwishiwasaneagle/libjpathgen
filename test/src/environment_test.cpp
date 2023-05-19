// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <jpathgen/environment.h>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace jpathgen::environment;
using Catch::Matchers::WithinRel;

inline void _vec_is_same_as_simple(MultiModalBivariateGaussian &mmbg, int n, int m)
{
  EigenMatrixXdRowMajor xv, yv;
  xv = EigenMatrixXdRowMajor::Random(n, m);
  yv = EigenMatrixXdRowMajor::Random(n, m);

  EigenMatrixXdRowMajor zv;
  zv = mmbg(xv, yv);

  double zv_non_vec, zv_act, xij, yij;
  for (int i = 0; i < xv.cols(); i++)
  {
    for (int j = 0; j < xv.rows(); j++)
    {
      xij = xv(j, i);
      yij = yv(j, i);
      zv_non_vec = mmbg(xij, yij);
      zv_act = zv(i, j);
      CHECK_THAT(zv_non_vec, WithinRel(zv_act));
    }
  }
}

inline void _maxima_at_x_y(MultiModalBivariateGaussian &mmbg, double x, double y, int N, double width){
  double maxima_value = mmbg(x,y);
  for (double xi = x-width; xi <= x+width; xi += width/N)
  {
    for (double yi = y-width; yi <= y+width; yi += width/N)
    {
      CHECK(mmbg(xi, yi) <= maxima_value);
    }
  }
}


SCENARIO("MVBivarGaussians can be instatiated", "[MMBG]")
{
  GIVEN("Using a std vector with pair")
  {
    STLMUS mus = { { 0, 0 } };
    STLCOVS covs = { { 1, 0 }, { 0, 1 } };

    MultiModalBivariateGaussian mmbg(mus, covs);
    WHEN("One mode is added")
    {
      THEN("It works without error")
      {
        REQUIRE(mmbg.length() == 1);
      }
      THEN("The value is 1/ ( 2 PI )")
      {
        double act = mmbg(0, 0);
        REQUIRE_THAT(act, WithinRel(1 / (2 * M_PI)));
      }

      THEN("It works with matrix inputs")
      {
        auto n = GENERATE(1, 2, 3);
        auto m = GENERATE(1, 2, 3);
        _vec_is_same_as_simple(mmbg, n, m);
      }
    }
  }

  GIVEN("1 mode at (0,0)")
  {
    MU mu = MU::Zero();
    COV cov = COV::Identity();

    REQUIRE(mu.sum() == 0);
    REQUIRE(cov.sum() == 2);

    MultiModalBivariateGaussian mmbg(mu, cov);

    WHEN("The length is evaluated")
    {
      THEN("The value is 1")
      {
        REQUIRE(mmbg.length() == 1);
      }
    }

    WHEN("MVBG is evaluated at (0,0)")
    {
      double const exp = 1 / (2 * M_PI);

      THEN("The value is 1/ ( 2 PI )")
      {
        double act = mmbg(0, 0);

        REQUIRE_THAT(act, WithinRel(exp));
      }
      THEN("It is at a maxima ")
      {
        _maxima_at_x_y(mmbg, 0,0, 3, 2);
      }
    }
  }
  GIVEN("4 modes at (0,0)")
  {
    MUS mus = Eigen::Matrix<double, 4, 2>::Zero();
    COV cov = COV::Identity();
    COVS covs = (Eigen::Matrix<double, 8, 2>() << cov, cov, cov, cov).finished();

    REQUIRE(mus.sum() == 0);
    REQUIRE(covs.sum() == 8);

    MultiModalBivariateGaussian mmbg(mus, covs);

    WHEN("The length is evaluated")
    {
      THEN("The value is 4")
      {
        REQUIRE(mmbg.length() == 4);
      }
    }

    WHEN("The MMBG is evaluated")
    {
      THEN("It works with matrix inputs")
      {
        auto n = GENERATE(1, 2, 3);
        auto m = GENERATE(1, 2, 3);
        _vec_is_same_as_simple(mmbg, n, m);
      }

      double const exp = 4 / (2 * M_PI);

      THEN("The value is 4/ ( 2 PI ) at (0,0)")
      {
        double act = mmbg(0, 0);

        REQUIRE_THAT(act, WithinRel(exp));
      }
      THEN("It is at a maxima at (0,0)")
      {
        _maxima_at_x_y(mmbg, 0,0, 3, 2);
      }
    }
  }
}

SCENARIO("MVBivarGaussians throws expected exceptions", "[MMBG]")
{
  GIVEN("Incompatible mus and covs shapes are passed")
  {
    WHEN("The number of covs is lower than the number of means")
    {
      MUS mus = Eigen::Matrix<double, 4, 2>::Zero();
      COVS covs = Eigen::Matrix<double, 4, 2>::Zero();
      THEN("An exception is thrown")
      {
        REQUIRE_THROWS(MultiModalBivariateGaussian(mus, covs));
      }
    }
    WHEN("The number of covs is higher than the number of means")
    {
      MUS mus = Eigen::Matrix<double, 2, 2>::Zero();
      COVS covs = Eigen::Matrix<double, 8, 2>();
      THEN("An exception is thrown")
      {
        REQUIRE_THROWS(MultiModalBivariateGaussian(mus, covs));
      }
    }
  }
}
