// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#include <pybind11/functional.h>
#include <pybind11/operators.h>

#include "jpathgen/environment.h"
#include "jpathgen/integration.h"
#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;
using namespace jpathgen::integration;
using namespace jpathgen::function;
using namespace jpathgen::environment;

PYBIND11_MODULE(_libjpathgen, m)
{
  m.doc() = "A C++ library to speed up jpathgen computations";

  py::class_<MultiModalBivariateGaussian>(m, "MultiModalBivariateGaussian")
      .def(py::init<Eigen::Ref<MUS>, Eigen::Ref<COVS>>())
      .def(py::init<STLMUS, STLCOVS>())
      .def("__len__", [](MultiModalBivariateGaussian& mmbg) { return mmbg.length(); })
      .def("__call__", [](MultiModalBivariateGaussian& mmbg, double x, double y) { return mmbg(x, y); })
      .def_property_readonly("_mus", &MultiModalBivariateGaussian::getMus)
      .def_property_readonly("_covs", &MultiModalBivariateGaussian::getCovs);

  m.def(
      "integrate_over_buffered_line",
      static_cast<double (*)(Function, jpathgen::geometry::STLCoords, double)>(&integrate_over_buffered_line));
  m.def(
      "integrate_over_buffered_line",
      static_cast<double (*)(Function, jpathgen::geometry::EigenCoords , double)>(&integrate_over_buffered_line));
  m.def(
      "integrate_over_buffered_line",
      static_cast<double (*)(MultiModalBivariateGaussian, jpathgen::geometry::STLCoords, double)>(&integrate_over_buffered_line));
  m.def(
      "integrate_over_buffered_line",
      static_cast<double (*)(MultiModalBivariateGaussian, jpathgen::geometry::EigenCoords , double)>(&integrate_over_buffered_line));
}