#   Copyright 2023 Jan-Hendrik Ewers
#   SPDX-License-Identifier: GPL-3.0-only
import pytest
import numpy as np
from libjpathgen import MultiModalBivariateGaussian


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
