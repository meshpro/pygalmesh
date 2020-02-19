import os

import helpers
import pygalmesh


def test_volume_from_surface():
    this_dir = os.path.dirname(os.path.abspath(__file__))
    mesh = pygalmesh.generate_volume_mesh_from_surface_mesh(
        os.path.join(this_dir, "meshes", "elephant.vtu"),
        facet_angle=25.0,
        facet_size=0.15,
        facet_distance=0.008,
        cell_radius_edge_ratio=3.0,
        verbose=False,
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 0.357612477657) < tol
    assert abs(min(mesh.points[:, 0]) + 0.358747130015) < tol
    assert abs(max(mesh.points[:, 1]) - 0.496137874959) < tol
    assert abs(min(mesh.points[:, 1]) + 0.495301051456) < tol
    assert abs(max(mesh.points[:, 2]) - 0.298780230629) < tol
    assert abs(min(mesh.points[:, 2]) + 0.300472866512) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 0.044164693065) < tol
