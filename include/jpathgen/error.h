/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef JPATHGEN_ERROR_H
#define JPATHGEN_ERROR_H

namespace jpathgen
{
  inline void Error(int b, const char *message)
  {
    if (b)
    {
      throw message;
    };
  }
}  // namespace jpathgen
#endif  // JPATHGEN_ERROR_H
