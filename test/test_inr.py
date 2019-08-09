import os

import helpers
import pygalmesh


def test_inr():
    this_dir = os.path.dirname(os.path.abspath(__file__))
    mesh = pygalmesh.generate_from_inr(
        os.path.join(this_dir, "meshes", "liver.inr"), cell_size=5.0, verbose=False
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 2.385709228515625e02) < tol
    assert abs(min(mesh.points[:, 0]) - 3.307421875000000e01) < tol
    assert abs(max(mesh.points[:, 1]) - 1.947126770019531e02) < tol
    assert abs(min(mesh.points[:, 1]) - 2.314493751525879e01) < tol
    assert abs(max(mesh.points[:, 2]) - 1.957076873779297e02) < tol
    assert abs(min(mesh.points[:, 2]) - 1.365468883514404e01) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    ref = 1.759104602743268e06
    assert abs(vol - ref) < ref * 1.0e-3
    return
