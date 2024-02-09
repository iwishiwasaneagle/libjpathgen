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
      _bgs.reserve(N);
      MU _mu;
      COV _cov;
      for (int i = 0; i < N; i++)
      {
        _mu = _mus.row(i);
        _cov = _covs.block<2, 2>(2 * i, 0);
        _bgs.emplace_back(_mu, _cov);
      }
    }

    double MultiModalBivariateGaussian::operator()(double x, double y)
    {
      double total = 0;
      for (BivariateGaussian& _bg : _bgs)
      {
        total += _bg(x, y);
      }
      return total / N;
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