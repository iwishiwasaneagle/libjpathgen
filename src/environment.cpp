/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "jpathgen/environment.h"

#include <cmath>

#include "jpathgen/error.h"

namespace jpathgen
{
  namespace environment
  {
    BivariateGaussian::BivariateGaussian(MU mu, COV cov)
        : mu_x(mu(0)),
          mu_y(mu(1)),
          sigma_x(sqrt(cov(0, 0))),
          sigma_y(sqrt(cov(1, 1))),
          rho(cov(0, 1) / (sigma_x * sigma_y))

    {
      a = (1 - (rho * rho));
      b = (1 / (2 * M_PI * sigma_x * sigma_y * sqrt(a)));
      c = (1 / (-2 * a));
    }
    double BivariateGaussian::operator()(double x, double y)
    {
      double d, e, f;

      d = (x - mu_x) / sigma_x;
      e = (y - mu_y) / sigma_y;
      f = (d * d) - 2 * rho * d * e + (e * e);

      return b * exp(c * f);
    }

    MultiModalBivariateGaussian::MultiModalBivariateGaussian(Eigen::Ref<MUS> mus, Eigen::Ref<COVS> covs)
        : _mus(mus),
          _covs(covs)
    {
      init();
    }

    MultiModalBivariateGaussian::MultiModalBivariateGaussian(STLMUS mus, STLCOVS covs)
    {
      _mus = Eigen::Map<MUS>(&(mus[0].first), static_cast<long>(mus.size()), 2).cast<double>();
      _covs = Eigen::Map<COVS>(&(covs[0].first), static_cast<long>(covs.size()), 2).cast<double>();
      init();
    }

    void MultiModalBivariateGaussian::init()
    {
      Error(2 * _mus.rows() != _covs.rows(), "mus and covs must be the same length");
      N = _mus.rows();
      constants.resize(N, 7);

      constants.leftCols(2) = _mus;  // mu_x, mu_y

      auto cov_xx = _covs.col(0)(Eigen::seq(0, 2 * N - 1, 2));
      auto cov_yy = _covs.col(1)(Eigen::seq(0, 2 * N - 1, 2));
      auto cov_xy = _covs(Eigen::seq(1, 2 * N, 2), 0);

      auto sigma_x = cov_xx.array().sqrt();  // sigma_x
      auto sigma_y = cov_yy.array().sqrt();  // sigma_y

      auto rho = cov_xy.array() / constants.col(0).cwiseProduct(constants.col(1)).array();  // rho

      constants.col(2) = sigma_x;
      constants.col(3) = sigma_y;
      constants.col(4) = rho;

      a = 1 - constants.col(4).array().square();
      constants.col(5) = (1 / (2 * M_PI * constants.col(2).array() * constants.col(3).array() * a.sqrt()));  // b
      constants.col(6) = (1 / (-2 * a));                                                                     // c
    }

    double MultiModalBivariateGaussian::operator()(double x, double y)
    {
      d = (x - constants.col(0).array()) / constants.col(2).array();
      e = (y - constants.col(1).array()) / constants.col(3).array();
      f = (d * d) - 2 * constants.col(4).array() * d * e + e.square();
      g = Eigen::exp(constants.col(6).array() * f);
      return (constants.col(5).array() * g).sum() / N;
    }

    int MultiModalBivariateGaussian::length() const
    {
      return N;
    }
    const MUS& MultiModalBivariateGaussian::getMus() const
    {
      return _mus;
    }
    const COVS& MultiModalBivariateGaussian::getCovs() const
    {
      return _covs;
    }
  }  // namespace environment
}  // namespace jpathgen