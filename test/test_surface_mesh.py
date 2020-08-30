import helpers
import numpy

import pygalmesh


def test_sphere():
    radius = 1.0
    s = pygalmesh.Ball([0.0, 0.0, 0.0], radius)
    mesh = pygalmesh.generate_surface_mesh(
        s, angle_bound=30, radius_bound=0.1, distance_bound=0.1, verbose=False
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - radius) < tol
    assert abs(min(mesh.points[:, 0]) + radius) < tol
    assert abs(max(mesh.points[:, 1]) - radius) < tol
    assert abs(min(mesh.points[:, 1]) + radius) < tol
    assert abs(max(mesh.points[:, 2]) - radius) < tol
    assert abs(min(mesh.points[:, 2]) + radius) < tol

    areas = helpers.compute_triangle_areas(mesh.points, mesh.get_cells_type("triangle"))
    surface_area = sum(areas)
    assert abs(surface_area - 4 * numpy.pi * radius ** 2) < 0.1
