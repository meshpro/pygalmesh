import os

import helpers
import pygalmesh


def test_inr():
    this_dir = os.path.dirname(os.path.abspath(__file__))
    mesh = pygalmesh.generate_from_inr(
        os.path.join(this_dir, "meshes", "skull_2.9.inr"), cell_size=5.0, verbose=False
    )

    tol = 1.0e-3
    ref = [2.031053e02, 3.739508e01, 2.425594e02, 2.558910e01, 2.300883e02, 1.775010e00]
    assert abs(max(mesh.points[:, 0]) - ref[0]) < tol * ref[0]
    assert abs(min(mesh.points[:, 0]) - ref[1]) < tol * ref[1]
    assert abs(max(mesh.points[:, 1]) - ref[2]) < tol * ref[2]
    assert abs(min(mesh.points[:, 1]) - ref[3]) < tol * ref[3]
    assert abs(max(mesh.points[:, 2]) - ref[4]) < tol * ref[4]
    assert abs(min(mesh.points[:, 2]) - ref[5]) < tol * ref[5]

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 2.725335e06
    # Debian needs 2.0e-2 here.
    # <https://github.com/nschloe/pygalmesh/issues/60>
    assert abs(vol - ref) < ref * 2.0e-2
