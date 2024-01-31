#!/bin/bash
#
# Copyright (c) 2024.  Jan-Hendrik Ewers
# SPDX-License-Identifier: GPL-3.0-only
#

set -eux

SRC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

INSTALL_LIBJPATHGEN=""
install_libjpathgen(){
  # LIBJPATHGEN
  cd $SRC_DIR
  mkdir _build
  cd _build
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  make -j "$(nproc)"
  make install
}

install_cubpackpp(){
  # CUBPACKPP
  cd $SRC_DIR
  git clone https://github.com/iwishiwasaneagle/cubpackpp
  cd cubpackpp
  git checkout f03b50f
  mkdir _build
  cd _build
  cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
  make -j "$(nproc)"
  make install
}

install_geos(){
  # GEOS
  cd $SRC_DIR
  curl -OL https://download.osgeo.org/geos/geos-3.12.1.tar.bz2
  tar xvfj geos-3.12.1.tar.bz2
  cd geos-3.12.1
  mkdir _build
  cd _build
  cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_DOCUMENTATION=OFF \
      -DBUILD_TESTING=OFF \
      ..
  make -j "$(nproc)"
  make install
}

install_eigen3(){
  # EIGEN3
  cd $SRC_DIR
  curl -OL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
  tar -xzvf eigen-3.4.0.tar.gz
  cd eigen-3.4.0
  mkdir _build
  cd _build
  cmake ..
  make install
}

for i in "$@"; do
  case $i in
    --libjpathgen)
      INSTALL_LIBJPATHGEN=1
      shift
      ;;
    --cubpackpp)
      install_cubpackpp
      shift
      ;;
    --eigen3)
      install_eigen3
      shift
      ;;
    --geos)
      install_geos
      shift
      ;;
    --all)
      install_cubpackpp
      install_eigen3
      install_geos
      install_libjpathgen
      ;;
    -*|--*)
      echo "Unknown option $i"
      exit 1
      ;;
    *)
      ;;
  esac
done

# Ensure libjpathgen is installed last
if [ -z "$INSTALL_LIBJPATHGEN" ]
then
  install_libjpathgen
fi
