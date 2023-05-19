// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#ifndef JPATHGEN_ENVIRONMENT_H
#define JPATHGEN_ENVIRONMENT_H

#include <Eigen/Core>
#include <functional>

namespace jpathgen
{
  namespace environment
  {

    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrixXdRowMajor;

    typedef Eigen::Matrix<double, 1, 2, Eigen::RowMajor> MU;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor> MUS;
    typedef Eigen::Matrix<double, 2,2, Eigen::RowMajor> COV;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 2 ,Eigen::RowMajor> COVS;

    typedef std::pair<double,double> STLMU;
    typedef std::vector<STLMU> STLMUS;
    typedef std::vector<STLMU> STLCOVS;

    class MultiModalBivariateGaussian
    {
     private:
      int N;
      MUS _mus;COVS _covs;

      EigenMatrixXdRowMajor single_bivar_gaussian(MU mu, COV cov, Eigen::Ref<EigenMatrixXdRowMajor> x, Eigen::Ref<EigenMatrixXdRowMajor>  y);
      double single_bivar_gaussian(MU mu, COV cov, double x, double  y);

     public:
      const MUS& getMus() const;
      const COVS& getCovs() const;


     protected:
      EigenMatrixXdRowMajor  eval_single_bivar_gaussian(int gauss_ind, Eigen::Ref<EigenMatrixXdRowMajor> x, Eigen::Ref<EigenMatrixXdRowMajor> y);
      double eval_single_bivar_gaussian(int gauss_ind, double x,double y);



     public:
      double operator()(double x, double y);
      EigenMatrixXdRowMajor operator()(Eigen::Ref<EigenMatrixXdRowMajor> x, Eigen::Ref<EigenMatrixXdRowMajor> y);
      int length();

      MultiModalBivariateGaussian(Eigen::Ref<MUS> mus, Eigen::Ref<COVS> covs);
      MultiModalBivariateGaussian(STLMUS mus, STLCOVS covs);
    };
  }  // namespace environment
}  // namespace jpathgen
#endif  // JPATHGEN_ENVIRONMENT_H
