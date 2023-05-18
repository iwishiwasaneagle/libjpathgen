// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include "jpathgen/environment.h"

#include <cmath>

#include "jpathgen/error.h"

namespace jpathgen{
  namespace environment
  {
    MultiModalBivariateGaussian::MultiModalBivariateGaussian(Eigen::Ref<MUS> mus, Eigen::Ref<COVS> covs):
    _mus(mus), _covs(covs)
    {
      N = _mus.rows();
      Error(2*N!=_covs.rows(), "mus and covs must be the same length");
    }

    MultiModalBivariateGaussian::MultiModalBivariateGaussian(STLMUS mus, STLCOVS covs)
    {

      _mus = Eigen::Map<MUS>(&(mus[0].first), mus.size(), 2).cast<double>();
      _covs = Eigen::Map<COVS>(&(covs[0].first), covs.size(), 2).cast<double>();
      N = _mus.rows();
      Error(2*N!=_covs.rows(), "mus and covs must be the same length");
    }


    inline double _single_bivar_gaussian(MU mu, COV cov, double x, double y){
      const double sigma_x = sqrt(cov(0,0));
      const double sigma_y = sqrt(cov(1,1));
      const double rho = cov(0,1)/(sigma_x*sigma_y);
      const double mu_x = mu(0);
      const double mu_y = mu(1);

      double a;
      double b;
      double c;
      double d;
      double e;
      double f;

       a = 1-(rho*rho);
       b = 1/(2*M_PI*sigma_x*sigma_y*sqrt(a));
       c = 1/(-2*a);
       d = (x-mu_x)/sigma_x;
       e = (y-mu_y)/sigma_y;
      f = (d*d)-2*rho*d*e+(e*e);

      return b* exp(c*f);
    }

    double MultiModalBivariateGaussian::eval_single_bivar_gaussian(int gauss_ind, double x, double y)
    {
      MU mu = _mus.row(gauss_ind);
      COV cov = _covs.block<2,2>(2*gauss_ind, 0);

      return _single_bivar_gaussian(mu, cov, x, y);
    }

    double MultiModalBivariateGaussian::operator()(double x, double y)
    {
      double total = 0;
      for (int i =0; i<N; i++){
        total+= eval_single_bivar_gaussian(i, x, y);
      }
      return total;
    }

    int MultiModalBivariateGaussian::length()
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
  }
}