cmake_minimum_required(VERSION 3.16)
project(
        ${CMAKE_PROJECT_NAME}Fuzzing
        LANGUAGES CXX
)

find_package(cifuzz NO_SYSTEM_ENVIRONMENT_PATH)
enable_fuzz_testing()
verbose_message("Adding fuzzing tests under ${CMAKE_PROJECT_NAME}Fuzzing...")

foreach(file ${fuzz_sources})
    string(REGEX REPLACE "\\.[^.]*$" "" fuzz_name ${file})

    add_fuzz_test(${fuzz_name}_Fuzzing ${file})

    set(${CMAKE_PROJECT_NAME}_TEST_LIB ${LIB_NAME})

    target_link_libraries(
            ${fuzz_name}_Fuzzing
            PRIVATE
            Eigen3::Eigen
            ${${CMAKE_PROJECT_NAME}_TEST_LIB}
    )
endforeach()

verbose_message("Finished adding fuzzing tests for ${CMAKE_PROJECT_NAME}.")
