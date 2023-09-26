# libjpathgen

[![CI](https://github.com/iwishiwasaneagle/libjpathgen/actions/workflows/CI.yml/badge.svg)](https://github.com/iwishiwasaneagle/libjpathgen/actions/workflows/CI.yml)
[![codecov](https://codecov.io/gh/iwishiwasaneagle/libjpathgen/graph/badge.svg?token=8FT3BH2K4S)](https://codecov.io/gh/iwishiwasaneagle/libjpathgen)

A python library written in C++ to aid in my research.

## Installation

```bash
sudo apt-get install \
      libeigen3-dev \
      pybind11-dev \
      libgeos++-dev \
      libgeos-dev \
      libgeos3.11.1

pip install git+https://github.com/iwishiwasaneagle/libjpathgen.git
```

## Run tests

```bash
cmake -B build \
  -DJPATHGEN_ENABLE_UNIT_TESTING=ON \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j $(nproc)
ctest --test-dir build
```

### With coverage

Add `-DJPATHGEN_ENABLE_CODE_COVERAGE=ON` to the initial cmake call.

```bash
lcov --capture --directory build --output-file coverage.info
lcov --remove coverage.info --output-file coverage.info '/usr/*' '*/test/*' '*/extern/*' '*/geos/*'
genhtml coverage.info -o build/html
firefox build/html/index.html
```