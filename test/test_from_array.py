import helpers
import numpy as np

import pygalmesh


def test_from_array():
    n = 200
    shape = (n, n, n)
    h = [1.0 / s for s in shape]
    vol = np.zeros(shape, dtype=np.uint16)
    i, j, k = np.arange(shape[0]), np.arange(shape[1]), np.arange(shape[2])
    ii, jj, kk = np.meshgrid(i, j, k)
    vol[ii * ii + jj * jj + kk * kk < n ** 2] = 1
    vol[ii * ii + jj * jj + kk * kk < (0.5 * n) ** 2] = 2

    mesh = pygalmesh.generate_from_array(
        vol, h, cell_size=100 * min(h), facet_distance=min(h), verbose=False
    )

    tol = min(h)
    ref = [1.0, 0.0, 1.0, 0.0, 1.0, 0.0]
    assert abs(max(mesh.points[:, 0]) - ref[0]) < tol
    assert abs(min(mesh.points[:, 0]) - ref[1]) < tol
    assert abs(max(mesh.points[:, 1]) - ref[2]) < tol
    assert abs(min(mesh.points[:, 1]) - ref[3]) < tol
    assert abs(max(mesh.points[:, 2]) - ref[4]) < tol
    assert abs(min(mesh.points[:, 2]) - ref[5]) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 1.0 / 6.0 * np.pi
    # Debian needs 2.0e-2 here.
    # <https://github.com/nschloe/pygalmesh/issues/60>
    assert abs(vol - ref) < ref * 2.0e-2


def test_from_array_with_subdomain_sizing():
    n = 200
    shape = (n, n, n)
    h = [1.0 / s for s in shape]
    vol = np.zeros(shape, dtype=np.uint16)
    i, j, k = np.arange(shape[0]), np.arange(shape[1]), np.arange(shape[2])
    ii, jj, kk = np.meshgrid(i, j, k)
    vol[ii * ii + jj * jj + kk * kk < n ** 2] = 1
    vol[ii * ii + jj * jj + kk * kk < (0.5 * n) ** 2] = 2

    mesh = pygalmesh.generate_from_array(
        vol,
        h,
        cell_size={1: 100 * min(h), 2: 10 * min(h)},
        facet_distance=min(h),
        verbose=False,
    )

    tol = min(h)
    ref = [1.0, 0.0, 1.0, 0.0, 1.0, 0.0]
    assert abs(max(mesh.points[:, 0]) - ref[0]) < tol
    assert abs(min(mesh.points[:, 0]) - ref[1]) < tol
    assert abs(max(mesh.points[:, 1]) - ref[2]) < tol
    assert abs(min(mesh.points[:, 1]) - ref[3]) < tol
    assert abs(max(mesh.points[:, 2]) - ref[4]) < tol
    assert abs(min(mesh.points[:, 2]) - ref[5]) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 1.0 / 6.0 * np.pi
    # Debian needs 2.0e-2 here.
    # <https://github.com/nschloe/pygalmesh/issues/60>
    assert abs(vol - ref) < ref * 2.0e-2
