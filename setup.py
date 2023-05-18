#   Copyright 2023 Jan-Hendrik Ewers
#   SPDX-License-Identifier: GPL-3.0-only

"""
    Setup file for libjpathgen.
"""
import sys

from skbuild import setup

if __name__ == "__main__":
    try:
        setup(
            packages=['libjpathgen'],
            package_dir={'': 'src'},
            cmake_install_dir='src/libjpathgen'
        )
    except:  # noqa
        print(
            "\n\nAn error occurred while building the project, "
            "please ensure you have the most updated version of setuptools, "
            "setuptools_scm, skbuild, cmake, and pybind11 with:\n"
            "   pip install -U setuptools setuptools_scm scikit-build cmake pybind11"
            "\n\n",
            file=sys.stderr,
        )
        raise