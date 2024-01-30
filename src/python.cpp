/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
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
using namespace jpathgen::geometry;

PYBIND11_MODULE(_libjpathgen, m)
{
  m.doc() = "A C++ library to speed up jpathgen computations";

  py::class_<MultiModalBivariateGaussian>(m, "MultiModalBivariateGaussian")
      .def(py::init<Eigen::Ref<MUS>, Eigen::Ref<COVS>>())
      .def(py::init<STLMUS, STLCOVS>())
      .def("__len__", [](MultiModalBivariateGaussian& mmbg) { return mmbg.length(); })
      .def("__call__", [](MultiModalBivariateGaussian& mmbg, double x, double y) { return mmbg(x, y); })
      .def("__call__", py::vectorize([](MultiModalBivariateGaussian& mmbg, double x, double y) { return mmbg(x, y); }))
      .def(
          "__repr__",
          [](MultiModalBivariateGaussian& mmbg)
          {
            std::ostringstream ss;
            ss << "<MultiModalBivariateGaussian(N=" << mmbg.getMus().rows() << " modes)>";
            return ss.str();
          })
      .def_property_readonly("_mus", &MultiModalBivariateGaussian::getMus)
      .def_property_readonly("_covs", &MultiModalBivariateGaussian::getCovs);

  py::class_<Args>(m, "Args").def(py::init<double>()).def_property_readonly("buffer_radius_m", &Args::get_buffer_radius_m);

  py::class_<DiscreteArgs, Args>(m, "DiscreteArgs")
      .def(py::init<double, int, int, double, double, double, double>())
      .def_property_readonly("N", &DiscreteArgs::get_N)
      .def_property_readonly("M", &DiscreteArgs::get_M)
      .def_property_readonly("minx", &DiscreteArgs::get_minx)
      .def_property_readonly("maxx", &DiscreteArgs::get_maxx)
      .def_property_readonly("minx", &DiscreteArgs::get_miny)
      .def_property_readonly("maxx", &DiscreteArgs::get_maxy);

  py::class_<ContinuousArgs, Args>(m, "ContinuousArgs")
      .def(py::init<double, double, double>())
      .def_property_readonly("abs_err_req", &ContinuousArgs::get_abs_err_req)
      .def_property_readonly("rel_err_req", &ContinuousArgs::get_rel_err_req);

  m.def(
      "continuous_integration_over_polygon",
      static_cast<double (*)(Function, STLCoords, ContinuousArgs*)>(&continuous_integration_over_polygon));
  m.def(
      "continuous_integration_over_polygon",
      static_cast<double (*)(MultiModalBivariateGaussian, STLCoords, ContinuousArgs*)>(
          &continuous_integration_over_polygon));
  m.def(
      "discrete_integration_over_polygon",
      static_cast<double (*)(Function, STLCoords, DiscreteArgs*)>(&discrete_integration_over_polygon));
  m.def(
      "discrete_integration_over_polygon",
      static_cast<double (*)(MultiModalBivariateGaussian, STLCoords, DiscreteArgs*)>(&discrete_integration_over_polygon));

  m.def(
      "continuous_integration_over_rectangle",
      static_cast<double (*)(Function, double, double, double, double, ContinuousArgs*)>(
          &continuous_integration_over_rectangle));
  m.def(
      "continuous_integration_over_rectangle",
      static_cast<double (*)(MultiModalBivariateGaussian, double, double, double, double, ContinuousArgs*)>(
          &continuous_integration_over_rectangle));
  m.def(
      "discrete_integration_over_rectangle",
      static_cast<double (*)(Function, double, double, double, double, DiscreteArgs*)>(
          &discrete_integration_over_rectangle));
  m.def(
      "discrete_integration_over_rectangle",
      static_cast<double (*)(MultiModalBivariateGaussian, double, double, double, double, DiscreteArgs*)>(
          &discrete_integration_over_rectangle));

  m.def(
      "continuous_integration_over_path",
      static_cast<double (*)(Function, STLCoords, ContinuousArgs*)>(&continuous_integration_over_path));
  m.def(
      "continuous_integration_over_path",
      static_cast<double (*)(Function, EigenCoords, ContinuousArgs*)>(&continuous_integration_over_path));
  m.def(
      "continuous_integration_over_path",
      static_cast<double (*)(MultiModalBivariateGaussian, STLCoords, ContinuousArgs*)>(&continuous_integration_over_path));
  m.def(
      "continuous_integration_over_path",
      static_cast<double (*)(MultiModalBivariateGaussian, EigenCoords, ContinuousArgs*)>(&continuous_integration_over_path));
  m.def(
      "discrete_integration_over_path",
      static_cast<double (*)(Function, STLCoords, DiscreteArgs*)>(&discrete_integration_over_path));
  m.def(
      "discrete_integration_over_path",
      static_cast<double (*)(Function, EigenCoords, DiscreteArgs*)>(&discrete_integration_over_path));
  m.def(
      "discrete_integration_over_path",
      static_cast<double (*)(MultiModalBivariateGaussian, STLCoords, DiscreteArgs*)>(&discrete_integration_over_path));
  m.def(
      "discrete_integration_over_path",
      static_cast<double (*)(MultiModalBivariateGaussian, EigenCoords, DiscreteArgs*)>(&discrete_integration_over_path));

  m.def(
      "continuous_integration_over_paths",
      static_cast<double (*)(Function, std::vector<STLCoords>, ContinuousArgs*)>(&continuous_integration_over_paths));
  m.def(
      "continuous_integration_over_paths",
      static_cast<double (*)(Function, std::vector<EigenCoords>, ContinuousArgs*)>(&continuous_integration_over_paths));
  m.def(
      "continuous_integration_over_paths",
      static_cast<double (*)(MultiModalBivariateGaussian, std::vector<STLCoords>, ContinuousArgs*)>(&continuous_integration_over_paths));
  m.def(
      "continuous_integration_over_paths",
      static_cast<double (*)(MultiModalBivariateGaussian, std::vector<EigenCoords>, ContinuousArgs*)>(&continuous_integration_over_paths));
  m.def(
      "discrete_integration_over_paths",
      static_cast<double (*)(Function, std::vector<STLCoords>, DiscreteArgs*)>(&discrete_integration_over_paths));
  m.def(
      "discrete_integration_over_paths",
      static_cast<double (*)(Function, std::vector<EigenCoords>, DiscreteArgs*)>(&discrete_integration_over_paths));
  m.def(
      "discrete_integration_over_paths",
      static_cast<double (*)(MultiModalBivariateGaussian, std::vector<STLCoords>, DiscreteArgs*)>(&discrete_integration_over_paths));
  m.def(
      "discrete_integration_over_paths",
      static_cast<double (*)(MultiModalBivariateGaussian, std::vector<EigenCoords>, DiscreteArgs*)>(&discrete_integration_over_paths));
}