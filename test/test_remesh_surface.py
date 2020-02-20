import os

import helpers
import pygalmesh


def test_inr():
    this_dir = os.path.dirname(os.path.abspath(__file__))
    mesh = pygalmesh.remesh_surface(
        os.path.join(this_dir, "meshes", "lion-head.off"),
        edge_size=0.025,
        facet_angle=25,
        facet_size=0.1,
        facet_distance=0.001,
        verbose=False,
    )

    tol = 1.0e-3
    ref = [
        3.705640e-01,
        -3.711630e-01,
        4.754940e-01,
        -4.728260e-01,
        4.998690e-01,
        -4.998350e-01,
    ]
    assert abs(max(mesh.points[:, 0]) - ref[0]) < tol * abs(ref[0])
    assert abs(min(mesh.points[:, 0]) - ref[1]) < tol * abs(ref[1])
    assert abs(max(mesh.points[:, 1]) - ref[2]) < tol * abs(ref[2])
    assert abs(min(mesh.points[:, 1]) - ref[3]) < tol * abs(ref[3])
    assert abs(max(mesh.points[:, 2]) - ref[4]) < tol * abs(ref[4])
    assert abs(min(mesh.points[:, 2]) - ref[5]) < tol * abs(ref[5])

    vol = sum(
        helpers.compute_triangle_areas(mesh.points, mesh.get_cells_type("triangle"))
    )
    ref = 1.917942005786831
    assert abs(vol - ref) < ref * 1.0e-3
