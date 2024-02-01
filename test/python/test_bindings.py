#  Copyright (c) 2024.  Jan-Hendrik Ewers
#  SPDX-License-Identifier: GPL-3.0-only
import pytest
import numpy as np
from libjpathgen import MultiModalBivariateGaussian, continuous_integration_over_rectangle, \
    discrete_integration_over_rectangle, ContinuousArgs, DiscreteArgs


@pytest.fixture(params=[np.eye(2)])
def covs(request):
    return request.param


@pytest.fixture(params=[np.zeros((1, 2))])
def mus(request):
    return request.param


@pytest.fixture
def mmbg(mus, covs):
    yield MultiModalBivariateGaussian(mus, covs)


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
    act = continuous_integration_over_rectangle(lambda x, y: 1, *bounds, ContinuousArgs(2.5, 0.05, 0))
    assert np.isclose(act, exp)


@pytest.mark.parametrize("bounds,exp", [
    [(0., 1., 0., 1.), 1.],
    [(0., 2., 0., 1.), 2.],
    [(0., 2., 0., 2.), 4.],
    [(1., 2., 0., 1.), 1.],
    [(100., 150., 50., 100.), 50. * 50.],
])
def test_discrete_integration_over_rectangle(bounds, exp):
    act = discrete_integration_over_rectangle(lambda x, y: 1, *bounds,
                                              DiscreteArgs(2.5, 1500, 1500, bounds[0] * 0.95, bounds[1] * 1.05,
                                                           bounds[2] * 0.95, bounds[3] * 1.05))
    assert np.isclose(act, exp, rtol=1e-1)
