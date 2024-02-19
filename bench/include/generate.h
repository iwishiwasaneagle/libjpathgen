/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */

//
// Created by jhewers on 07/02/24.
//

#ifndef GENERATE_H
#define GENERATE_H

jpathgen::geometry::EigenCoords generate_path(int Ncoords = 100)
{
  Eigen::MatrixXd angles = Eigen::MatrixXd::Random(Ncoords, 1);
  Eigen::MatrixX2d coords;
  coords.resize(Ncoords, 2);
  for (int i = 0; i < Ncoords; i++)
  {
    auto sliced_angles = angles.block(0, 0, i, 1).array();
    coords(i, 0) = sliced_angles.cos().sum();
    coords(i, 1) = sliced_angles.sin().sum();
  }
  return coords;
}

jpathgen::environment::MultiModalBivariateGaussian generate_mmbg(int N = 4)
{
  jpathgen::environment::MUS mus = jpathgen::environment::MUS::Random(N, 2);
  jpathgen::environment::COVS covs = jpathgen::environment::COVS::Random(2*N, 2);

  jpathgen::environment::MultiModalBivariateGaussian mmbg(mus, covs);

  return mmbg;
};

#endif  // GENERATE_H
