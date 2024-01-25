set(sources
        src/geometry.cpp
        src/integration.cpp
        src/environment.cpp
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
        src/integration_test.cpp
        )

set(fuzz_sources
        variable_length_gmm.cpp
        variable_length_path.cpp
        variable_length_gmm_and_path.cpp
)
