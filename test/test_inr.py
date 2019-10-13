import os

import helpers
import pygalmesh


def test_inr():
    this_dir = os.path.dirname(os.path.abspath(__file__))
    mesh = pygalmesh.generate_from_inr(
        os.path.join(this_dir, "meshes", "skull_2.9.inr"), cell_size=5.0, verbose=False
    )

    tol = 1.0e-3
    ref = [
        2.031053e+02,
        3.739508e+01,
        2.425594e+02,
        2.558910e+01,
        2.300883e+02,
        1.775010e+00,
    ]
    assert abs(max(mesh.points[:, 0]) - ref[0]) < tol * ref[0]
    assert abs(min(mesh.points[:, 0]) - ref[1]) < tol * ref[1]
    assert abs(max(mesh.points[:, 1]) - ref[2]) < tol * ref[2]
    assert abs(min(mesh.points[:, 1]) - ref[3]) < tol * ref[3]
    assert abs(max(mesh.points[:, 2]) - ref[4]) < tol * ref[4]
    assert abs(min(mesh.points[:, 2]) - ref[5]) < tol * ref[5]

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    ref = 2.725335e+06
    assert abs(vol - ref) < ref * 1.0e-3
    return
