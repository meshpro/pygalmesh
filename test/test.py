# -*- coding: utf-8 -*-
#
import loom

import numpy
import meshio


def compute_volumes(vertices, tets):
    cell_coords = vertices[tets]

    a = cell_coords[:, 1, :] - cell_coords[:, 0, :]
    b = cell_coords[:, 2, :] - cell_coords[:, 0, :]
    c = cell_coords[:, 3, :] - cell_coords[:, 0, :]

    # omega = <a, b x c>
    omega = numpy.einsum('ij, ij->i', a, numpy.cross(b, c))

    # https://en.wikipedia.org/wiki/Tetrahedron#Volume
    return abs(omega) / 6.0


def test_ball():
    s = loom.Ball([0, 0, 0], 1.0)
    bounding_sphere_radius = 5.0
    loom.generate_mesh(s, bounding_sphere_radius, 'out.mesh')

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    assert abs(max(vertices[:, 0]) - 1.0) < 0.02
    assert abs(min(vertices[:, 0]) + 1.0) < 0.02
    assert abs(max(vertices[:, 1]) - 1.0) < 0.02
    assert abs(min(vertices[:, 1]) + 1.0) < 0.02
    assert abs(max(vertices[:, 2]) - 1.0) < 0.02
    assert abs(min(vertices[:, 2]) + 1.0) < 0.02

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 4.0/3.0 * numpy.pi) < 0.15

    return


def test_balls():
    s0 = loom.Ball([0.5, 0, 0], 1.0)
    s1 = loom.Ball([-0.5, 0, 0], 1.0)
    u = loom.Union(s0, s1)
    loom.generate_mesh(u, 5.0, 'out.mesh')

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    assert abs(max(vertices[:, 0]) - 1.5) < 0.02
    assert abs(min(vertices[:, 0]) + 1.5) < 0.02
    assert abs(max(vertices[:, 1]) - 1.0) < 0.02
    assert abs(min(vertices[:, 1]) + 1.0) < 0.02
    assert abs(max(vertices[:, 2]) - 1.0) < 0.02
    assert abs(min(vertices[:, 2]) + 1.0) < 0.02

    vol = sum(compute_volumes(vertices, cells['tetra']))
    ref_vol = 6.92614543632
    assert abs(vol - ref_vol) < 0.1

    return


def test_cuboid():
    s0 = loom.Cuboid([0, 0, 0], [1, 2, 3])
    loom.generate_mesh(s0, 5.0, 'out.mesh')

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-10
    assert abs(max(vertices[:, 0]) - 1.0) < tol
    assert abs(min(vertices[:, 0]) + 0.0) < tol
    assert abs(max(vertices[:, 1]) - 2.0) < tol
    assert abs(min(vertices[:, 1]) + 0.0) < tol
    assert abs(max(vertices[:, 2]) - 3.0) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 6.0) < tol

    return


if __name__ == '__main__':
    test_ball()
