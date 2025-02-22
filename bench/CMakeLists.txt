cmake_minimum_required(VERSION 3.22)
project(
        ${CMAKE_PROJECT_NAME}Bench
        LANGUAGES CXX
)
verbose_message("Adding benchmark under ${CMAKE_PROJECT_NAME}Bench...")

include(FetchContent)
FetchContent_Declare(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG v4.3.11
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(nanobench)

foreach(file ${bench_sources})
    string(REGEX REPLACE "([a-zA-Z0-9_ ]+)(\.cpp)" "\\1" bench_name ${file})

    add_executable(${bench_name}_Bench ${file} ${bench_headers})

    target_include_directories(${bench_name}_Bench PRIVATE include)

    target_compile_features(${bench_name}_Bench PUBLIC cxx_std_17)

    target_link_libraries(
    ${bench_name}_Bench
            PRIVATE
            ${LIB_NAME}
            nanobench
    )
endforeach()
