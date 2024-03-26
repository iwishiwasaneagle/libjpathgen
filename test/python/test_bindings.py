#  Copyright (c) 2024.  Jan-Hendrik Ewers
#  SPDX-License-Identifier: GPL-3.0-only
import textwrap
import warnings

import pytest
import numpy as np
import libjpathgen

from typing import Type, Callable
import itertools
import re


@pytest.fixture(params=[np.eye(2)])
def covs(request):
    return request.param


@pytest.fixture(params=[np.zeros((1, 2))])
def mus(request):
    return request.param


@pytest.fixture
def mmbg(mus, covs):
    yield libjpathgen.MultiModalBivariateGaussian(mus, covs)


def test_MMBG_at_00(mmbg):
    assert np.isclose(mmbg(0, 0), 1 / (2 * np.pi))


def test_MMBG_00_is_maxima(mmbg):
    origin = mmbg(0, 0)
    xv, yv = np.meshgrid(np.linspace(-1, 1, 3), np.linspace(-1, 1, 3))

    for xi, yi in zip(xv.flatten(), yv.flatten()):
        if xi == 0 and yi == 0:
            continue
        assert mmbg(xi, yi) < origin


@pytest.mark.parametrize("inp", [np.ones((2, 20))])
def test_MMBG_vectorized_call(mmbg, inp):
    x, y = inp
    out = mmbg(x, y)
    assert len(out) == len(x)
    assert not np.isnan(out).any()


@pytest.mark.parametrize("inp", [np.meshgrid(np.linspace(0, 1, 10), np.linspace(0, 1, 10))])
def test_MMBG_vectorized_call_has_same_result(mmbg, inp):
    x, y = inp
    act = mmbg(x, y)

    exp = np.empty(x.shape)
    xis, yis = np.meshgrid(np.arange(len(x)), np.arange(len(y)))
    for xi, yi in zip(xis.flatten(), yis.flatten()):
        exp[xi, yi] = mmbg(x[xi, yi], y[xi, yi])

    assert np.allclose(exp, act)


@pytest.mark.parametrize("bounds,exp", [
    [(0., 1., 0., 1.), 1.],
    [(0., 2., 0., 1.), 2.],
    [(0., 2., 0., 2.), 4.],
    [(1., 2., 0., 1.), 1.],
    [(100., 200., 50., 150.), 100. * 100.],
])
def test_continuous_integration_over_rectangle(bounds, exp):
    act = libjpathgen.continuous_integration_over_rectangle(lambda x, y: 1, *bounds,
                                                            libjpathgen.ContinuousArgs(2.5, 0.05, 0))
    assert np.isclose(act, exp)


@pytest.mark.parametrize("bounds,exp", [
    [(0., 1., 0., 1.), 1.],
    [(0., 2., 0., 1.), 2.],
    [(0., 2., 0., 2.), 4.],
    [(1., 2., 0., 1.), 1.],
    [(100., 150., 50., 100.), 50. * 50.],
])
def testdiscrete_integration_over_rectangle(bounds, exp):
    act = libjpathgen.discrete_integration_over_rectangle(lambda x, y: 1, *bounds,
                                                          libjpathgen.DiscreteArgs(2.5, 1500, 1500, bounds[0] * 0.95,
                                                                                   bounds[1] * 1.05,
                                                                                   bounds[2] * 0.95, bounds[3] * 1.05))
    assert np.isclose(act, exp, rtol=1e-1)


def get_methods(cls: type, include_base: bool = True):
    fns_and_classes = []

    if include_base:
        fns_and_classes.append(cls)

    for f in filter(lambda f: (not f.startswith("_")) and f in cls.__dict__, dir(cls)):
        maybe_callable = cls.__dict__[f]

        if isinstance(maybe_callable, type):
            fns_and_classes.append(maybe_callable.__init__)
        elif isinstance(maybe_callable, Callable):
            fns_and_classes.append(maybe_callable)

    return fns_and_classes


@pytest.mark.parametrize("fn", itertools.chain.from_iterable(map(get_methods,
                                                                 get_methods(libjpathgen, include_base=False),
                                                                 ))
                         )
def test_args_and_kwargs_have_meaningful_names(fn):
    print(textwrap.dedent(f"""
    WARNING: Currently using __doc__ rather than inspect.getfullargspec as the latter doesn't support pybind11 fn. This 
             may cause inaccuracies. Be warned!
             
     {fn.__name__=}
     {fn.__doc__=}
    """))
    docs = fn.__doc__
    assert docs is not None
    args = re.findall(r'arg\d', docs)
    print(f"{args=}")
    assert len(args) == 0
