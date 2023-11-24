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

### Requirements

For general use the below is sufficient.

```bash
sudo apt-get install \
  libeigen3-dev \
  libgeos++-dev \
  libgeos-dev \
  libgeos3.11.1 \ # libgeos3.12.0 for ubuntu:23.10
  pybind11-dev
```

For testing, `Catch2>=v3.4.0` is required (see [CI.yml](.github/workflows/CI.yml))

#### libgeos

The minimum supported version of libgeos is `3.11.1`. This means that it must be built from source in `ubuntu 20.4` as
even `ppa:ubuntugis/ubuntugis-unstable` cannot help here (`3.10.2` is the maximum installable via `apt`).

#### pybind

The minimum version of pybind11 depends on the version of python. It is advised to either install from source or
using pip (`pip install "pybind11[global]>=2.11.0"`).

**WARNING**: `Ubuntu 22.04` only ships with `pybind11@v2.9.1`.

| Python | [`pybind11-dev`](https://pybind11.readthedocs.io/en/stable/changelog.html) |
|--------|----------------------------------------------------------------------------|
| 3.10   | 2.9.0                                                                      |
| 3.11   | 2.10.0                                                                     |
| 3.12   | 2.11.0                                                                     |

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
