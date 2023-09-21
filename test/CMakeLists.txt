cmake_minimum_required(VERSION 3.15)
project(
  ${CMAKE_PROJECT_NAME}Tests
  LANGUAGES CXX
)

verbose_message("Adding tests under ${CMAKE_PROJECT_NAME}Tests...")

find_package(Catch2 3 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
include(CTest)
include(Catch)

foreach(file ${test_sources})
  string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" test_name ${file})

  add_executable(${test_name}_Tests ${file})

  target_compile_features(${test_name}_Tests PUBLIC cxx_std_17)

  set(${CMAKE_PROJECT_NAME}_TEST_LIB ${LIB_NAME})

  target_link_libraries(
    ${test_name}_Tests
    PRIVATE
      Catch2::Catch2WithMain
          Eigen3::Eigen
      ${${CMAKE_PROJECT_NAME}_TEST_LIB}
  )
  catch_discover_tests(${test_name}_Tests)
endforeach()

if(1)

  add_executable(all_Tests ${test_sources})

  target_compile_features(all_Tests PUBLIC cxx_std_17)

  set(${CMAKE_PROJECT_NAME}_TEST_LIB ${LIB_NAME})

  if(${PROJECT_NAME_UPPERCASE}_ENABLE_CODE_COVERAGE)
    target_link_libraries(
            all_Tests
            PRIVATE
            Catch2::Catch2WithMain --coverage
            Eigen3::Eigen
            ${${CMAKE_PROJECT_NAME}_TEST_LIB}
    )
    target_compile_options(all_Tests PRIVATE --coverage)
  else ()
    target_link_libraries(
            all_Tests
            PRIVATE
            Catch2::Catch2WithMain
            Eigen3::Eigen
            ${${CMAKE_PROJECT_NAME}_TEST_LIB}
    )
  endif ()

  catch_discover_tests(all_Tests)
  endif ()

verbose_message("Finished adding unit tests for ${CMAKE_PROJECT_NAME}.")
