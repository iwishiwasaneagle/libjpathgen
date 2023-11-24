cmake_minimum_required(VERSION 3.16)
project(
        "jpathgen"
        VERSION 0.1.0
        LANGUAGES CXX
)
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif ()
include(GNUInstallDirs)
include(cmake/StandardSettings.cmake)
include(cmake/StaticAnalyzers.cmake)
include(cmake/Utils.cmake)
add_clang_format_target()
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif ()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")

if (UNIX)
    add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")    #this will allow to use same _DEBUG macro available in both Linux as well as Windows - MSCV environment. Easy to put Debug specific code.
endif (UNIX)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)

set(EXE_NAME ${PROJECT_NAME_UPPERCASE}_EXE)
set(LIB_NAME ${PROJECT_NAME_LOWERCASE})
message(STATUS ${LIB_NAME})
set(PY_NAME _lib${PROJECT_NAME_LOWERCASE})


find_package(Eigen3 CONFIG REQUIRED)

find_package(GEOS 3.11.1 CONFIG REQUIRED)
if (GEOS_VERSION LESS "3.12.0")
    message(STATUS "Geos version is ${GEOS_VERSION}<3.12.0 and therefore enabling compatibility mode.")
    add_compile_definitions(GEOS_COMPATIBILITY_REQUIRED)
endif ()

add_subdirectory(extern/cubpackpp)

# Find all headers and implementation files
if(1)
    include(cmake/SourcesAndHeaders.cmake)
    add_library(${LIB_NAME} STATIC SHARED ${sources} ${headers} ${templates})
    target_include_directories(
            ${LIB_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            PRIVATE
            ${PROJECT_SOURCE_DIR}/src
    )
    target_link_libraries(
            ${LIB_NAME}
            PUBLIC
            Eigen3::Eigen
            GEOS::geos
            cubpackpp::cubpackpp
    )
    if (${PROJECT_NAME_UPPERCASE}_ENABLE_VECTORIZATION)
        message(WARNING "Vectorization is in experimental mode and may cause unknown issues")
        include(cmake/EigenVectorization.cmake)
        target_compile_options(
                ${LIB_NAME}
            PRIVATE
                ${_CXX_VECTORIZATION_FLAGS}
        )
    endif ()

    install(
            TARGETS
            ${LIB_NAME}
            EXPORT
            ${LIB_NAME}Targets
            LIBRARY DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION
            ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION
            include
            PUBLIC_HEADER DESTINATION
            include
    )

    install(
            EXPORT
            ${LIB_NAME}Targets
            FILE
            ${LIB_NAME}Targets.cmake
            NAMESPACE
            ${LIB_NAME}::
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    )
    install(
            DIRECTORY
            include/${LIB_NAME}
            DESTINATION
            include
)
endif()

set(${LIB_NAME}_CMAKE_PACKAGE_INSTALL_SUBDIR "share/${LIB_NAME}/cmake")

if(1)
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${LIB_NAME}ConfigVersion.cmake
            VERSION
            ${PROJECT_VERSION}
            COMPATIBILITY
            SameMajorVersion
    )
    configure_package_config_file(
            ${CMAKE_CURRENT_LIST_DIR}/cmake/${LIB_NAME}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake
            INSTALL_DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    )
    install(
            FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    )
endif()

if (${${PROJECT_NAME_UPPERCASE}_BUILD_EXECUTABLE})
    add_executable(${EXE_NAME} ${exe_sources})
    find_package(CLI11 CONFIG REQUIRED)
    target_link_libraries(
            ${EXE_NAME}
            PRIVATE
            Eigen3::Eigen
            GEOS::geos
            cubpackpp::cubpackpp
            CLI11::CLI11
            ${LIB_NAME}
    )
    target_include_directories(
            ${EXE_NAME}
            PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    add_executable(${PROJECT_NAME}::${EXE_NAME} ALIAS ${EXE_NAME})
endif ()


#
# Install library for easy downstream inclusion
#



# We are using the SKBUILD variable, which is defined when scikit-build is
# running the CMake build, to control building the Python wrapper. This allows
# the C++ project to be installed, standalone, when using the standard CMake
# build flow.
if (DEFINED SKBUILD OR DEFINED DEV)
    # prevent an unused variable warning
    set(ignoreMe "${SKBUILD}")
    set(PYBIND11_FINDPYTHON ON)
    find_package(pybind11 CONFIG REQUIRED)
    pybind11_add_module(${PY_NAME} ${python_sources})

    target_include_directories(
            ${PY_NAME}
            PUBLIC
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(
            ${PY_NAME}
            PRIVATE
            Eigen3::Eigen
            GEOS::geos
            cubpackpp::cubpackpp
    )

    # Installing the extension module to the root of the package
    install(TARGETS ${PY_NAME} DESTINATION .)

    configure_file("${PROJECT_SOURCE_DIR}/src/lib${PROJECT_NAME_LOWERCASE}/__main__.py.in"
            "${PROJECT_BINARY_DIR}/src/lib${PROJECT_NAME_LOWERCASE}/__main__.py")

    install(FILES "${PROJECT_BINARY_DIR}/src/lib${PROJECT_NAME_LOWERCASE}/__main__.py" DESTINATION .)

    if (APPLE)
        set_target_properties(
                ${PY_NAME} PROPERTIES INSTALL_RPATH "@loader_path/${CMAKE_INSTALL_LIBDIR}")
    else ()
        set_target_properties(${PY_NAME} PROPERTIES INSTALL_RPATH
                "$ORIGIN/${CMAKE_INSTALL_LIBDIR}")
    endif ()

endif ()



if (${PROJECT_NAME_UPPERCASE}_ENABLE_UNIT_TESTING)
    enable_testing()
    message(STATUS "Build unit tests for the project. Tests should always be found in the test folder\n")
    add_subdirectory(test)
endif ()


if(${PROJECT_NAME_UPPERCASE}_ENABLE_FUZZING)
    message(STATUS "Build fuzzing tests for the project. Fuzzing tests should always be found in the test/fuzzing folder\n")
    add_subdirectory(test/fuzzing)
endif ()