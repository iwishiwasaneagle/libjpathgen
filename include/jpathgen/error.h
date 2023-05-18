// #  Copyright 2023 Jan-Hendrik Ewers
// #  SPDX-License-Identifier: GPL-3.0-only

#ifndef JPATHGEN_ERROR_H
#define JPATHGEN_ERROR_H
#include <iostream>

namespace jpathgen
{
  inline void Error(int b, const char *message)
  {
    if (b)
    {
      ::std::cerr << message << ::std::endl;
      ::std::cerr.flush();
      abort();
    };
  }
}  // namespace jpathgen
#endif  // JPATHGEN_ERROR_H
