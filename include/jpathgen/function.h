/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef JPATHGEN_FUNCTION_H
#define JPATHGEN_FUNCTION_H

#include <functional>

namespace jpathgen
{
  namespace function
  {
    typedef std::function<double(const double&, const double&)> Function;
  }
}  // namespace jpathgen
#endif  // JPATHGEN_FUNCTION_H
