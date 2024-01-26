/*
 * Copyright (c) 2024.  Jan-Hendrik Ewers
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef GEOS_COMPAT_H
#define GEOS_COMPAT_H

/*
libgeos3.12.0 and later introduced breaking changes (commit 60edf0e5f). However, this version has not found its way
into earlier Ubuntu versions (i.e. 23.04 and before). However, Ubuntu 23.10 and later do and as such
this flag is crucial for compatibility.

We define this using cmake like

if (GEOS_VERSION LESS "3.12.0")
    message(STATUS "Geos version is ${GEOS_VERSION}<3.12.0 and therefore enabling compatibility mode.")
    add_compile_definitions(GEOS_COMPATIBILITY_REQUIRED)
endif ()
*/
#ifdef GEOS_COMPATIBILITY_REQUIRED
  #include <geos/geom/CoordinateArraySequence.h>
  namespace geos::geom {
    using CoordinateSequenceCompat =  CoordinateArraySequence ;
  }
#else
#include <geos/geom/CoordinateSequence.h>
namespace geos::geom {
      using CoordinateSequenceCompat = CoordinateSequence;
  }
#endif

#endif // GEOS_COMPAT_H