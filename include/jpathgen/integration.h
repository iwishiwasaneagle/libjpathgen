// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#ifndef JDRONES_INTEGRATION_H
#define JDRONES_INTEGRATION_H

#include <cubpackpp/cubpackpp.h>

#include "jpathgen/geos_compat.h"


#include <Eigen/Core>
#include <memory>

#include "jpathgen/environment.h"
#include "jpathgen/geometry.h"
#include "jpathgen/function.h"

#define DEFAULT_D 2.5

namespace jpathgen
{
  namespace integration
  {

    template<typename Callable>
    double _conversion(Callable f, const cubpackpp::Point& pt);

    double _integration_over_region_collections(cubpackpp::Function fn, cubpackpp::REGION_COLLECTION rc);

    template<typename FUNC, typename COORDS>
    double integrate_over_buffered_line(FUNC f, COORDS coords, double d = DEFAULT_D);

    template<typename FUNC>
    double integrate_over_rect(FUNC f, double left, double right, double bottom, double top);
  }  // namespace integration
}  // namespace jpathgen
#endif  // JDRONES_INTEGRATION_H
