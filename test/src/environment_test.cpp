// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <jpathgen/environment.h>

#include <Eigen/Core>

using namespace jpathgen::environment;
using Catch::Matchers::WithinRel;

SCENARIO("MVBivarGaussians can be instatiated", "[MVBG]"){
  GIVEN("Using a std vector with pair"){
    STLMUS mus = {{0,0}};
    STLCOVS covs = {{1,0},{0,1}};

    WHEN("One mode is added"){
      THEN("It works without error"){

        MultiModalBivariateGaussian mmbg(mus, covs);
        REQUIRE(mmbg.length() == 1);
      }
      THEN("The value is 1/ ( 2 PI )")
      {
        MultiModalBivariateGaussian mmbg(mus, covs);
        double act = mmbg(0,0);
        REQUIRE_THAT(act, WithinRel(1/(2*M_PI)));
      }
    }
  }

  GIVEN("1 mode at (0,0)"){
    MU mu = MU::Zero();
    COV cov = COV::Identity();

    REQUIRE( mu.sum() == 0);
    REQUIRE( cov.sum() == 2);

    MultiModalBivariateGaussian mmbg(mu, cov);

    WHEN("The length is evaluated"){
      THEN("The value is 1"){
        REQUIRE(mmbg.length() == 1);
      }
    }

    WHEN("MVBG is evaluated at (0,0)"){
      double const exp = 1/(2*M_PI);

      THEN("The value is 1/ ( 2 PI )"){
          double act = mmbg(0,0);

       REQUIRE_THAT(act, WithinRel(exp));

      }
      THEN("It is at a maxima "){
       for(double xi=-1;xi<=1;xi+=0.5)
       {
         for(double yi=-1;yi<=1;yi+=0.5)
         {
           if (xi==0 && yi==0){
             continue;
           }
           CHECK(mmbg(xi,yi) < exp);
         }
       }
      }
    }
  }
  GIVEN("4 modes at (0,0)"){
    MUS mus = Eigen::Matrix<double, 4, 2>::Zero();
    COV cov = COV::Identity();
    COVS covs = (Eigen::Matrix<double, 8,2>() << cov,cov,cov,cov).finished();

    REQUIRE( mus.sum() == 0);
    REQUIRE( covs.sum() == 8);

    MultiModalBivariateGaussian mmbg(mus, covs);

    WHEN("The length is evaluated"){
      THEN("The value is 4"){
        REQUIRE(mmbg.length() == 4);
      }
    }

    WHEN("MVBG is evaluated at (0,0)"){
      double const exp = 1/(2*M_PI);

      THEN("The value is 1 / ( 2 PI )"){
        double act = mmbg(0,0);

        REQUIRE_THAT(act, WithinRel(exp));

      }
      THEN("It is at a maxima "){
        for(double xi=-1;xi<=1;xi+=0.5)
        {
          for(double yi=-1;yi<=1;yi+=0.5)
          {
            if (xi==0 && yi==0){
              continue;
            }
            CHECK(mmbg(xi,yi) < exp);
          }
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

