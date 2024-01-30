set(sources
        src/geometry.cpp
        src/integration/continuous.cpp
        src/integration/discrete.cpp
        src/environment.cpp
        src/geometry/coord_sequence_from_array.cpp
        )

set(exe_sources
        src/main.cpp
        ${sources}
        )

set(python_sources
        src/python.cpp
        )

set(headers
        include/jpathgen/geometry.h
        include/jpathgen/integration.h
        include/jpathgen/environment.h
        include/jpathgen/function.h
        include/jpathgen/error.h
        include/jpathgen/geos_compat.h
        )

set(test_sources
        src/environment_test.cpp
        src/integration/continuous_test.cpp
        src/integration/discrete_test.cpp
        )

set(fuzz_sources
        variable_length_gmm.cpp
        variable_length_path.cpp
        variable_length_gmm_and_path.cpp
)
