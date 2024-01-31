#!/bin/bash
#
# Copyright (c) 2024.  Jan-Hendrik Ewers
# SPDX-License-Identifier: GPL-3.0-only
#

set -eux

SRC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

INSTALL_LIBJPATHGEN=""
INSTALL_CUBPACKPP=""
INSTALL_GEOS=""
INSTALL_EIGEN3=""
for i in "$@"; do
  case $i in
    --libjpathgen)
      INSTALL_LIBJPATHGEN=1
      shift
      ;;
    --cubpackpp)
      INSTALL_CUBPACKPP=1
      shift
      ;;
    --eigen3)
      INSTALL_EIGEN3=1
      shift
      ;;
    --geos)
      INSTALL_GEOS=1
      shift
      ;;
    --all)
      INSTALL_CUBPACKPP=1
      INSTALL_EIGEN3=1
      INSTALL_GEOS=1
      INSTALL_LIBJPATHGEN=1
      ;;
    -*|--*)
      echo "Unknown option $i"
      exit 1
      ;;
    *)
      ;;
  esac
done

if [ -n "$INSTALL_CUBPACKPP" ]
then
  # CUBPACKPP
  cd $SRC_DIR
  git clone https://github.com/iwishiwasaneagle/cubpackpp
  cd cubpackpp
  git checkout f03b50f
  mkdir -p _build
  cd _build
  cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
  make -j "$(nproc)"
  make install
fi

if [ -n "$INSTALL_GEOS" ]
then
  # GEOS
  cd $SRC_DIR
  curl -OL https://download.osgeo.org/geos/geos-3.12.1.tar.bz2
  tar xvfj geos-3.12.1.tar.bz2
  cd geos-3.12.1
  mkdir -p _build
  cd _build
  cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_DOCUMENTATION=OFF \
      -DBUILD_TESTING=OFF \
      ..
  make -j "$(nproc)"
  make install
fi

if [ -n "$INSTALL_EIGEN3" ]
then
  # EIGEN3
  cd $SRC_DIR
  curl -OL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
  tar -xzvf eigen-3.4.0.tar.gz
  cd eigen-3.4.0
  mkdir -p _build
  cd _build
  cmake ..
  make install
fi

if [ -n "$INSTALL_LIBJPATHGEN" ]
then
  # LIBJPATHGEN
  cd $SRC_DIR
  mkdir -p _build
  cd _build
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  make -j "$(nproc)"
  make install
fi
