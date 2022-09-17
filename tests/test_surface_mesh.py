import helpers
import numpy as np

import pygalmesh


def test_sphere():
    radius = 1.0
    s = pygalmesh.Ball([0.0, 0.0, 0.0], radius)
    mesh = pygalmesh.generate_surface_mesh(
        s,
        min_facet_angle=30.0,
        max_radius_surface_delaunay_ball=0.1,
        max_facet_distance=0.1,
        verbose=False,
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - radius) < (1.0 + radius) * tol
    assert abs(min(mesh.points[:, 0]) + radius) < (1.0 + radius) * tol
    assert abs(max(mesh.points[:, 1]) - radius) < (1.0 + radius) * tol
    assert abs(min(mesh.points[:, 1]) + radius) < (1.0 + radius) * tol
    assert abs(max(mesh.points[:, 2]) - radius) < (1.0 + radius) * tol
    assert abs(min(mesh.points[:, 2]) + radius) < (1.0 + radius) * tol

    areas = helpers.compute_triangle_areas(mesh.points, mesh.get_cells_type("triangle"))
    surface_area = sum(areas)
    assert abs(surface_area - 4 * np.pi * radius**2) < 0.1
