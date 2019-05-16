# -*- coding: utf-8 -*-
#
import pygalmesh

import helpers


def test_inr():
    helpers.download("liver.inr", "c69513a79231881942a86df56d41090e")
    mesh = pygalmesh.generate_from_inr("/tmp/liver.inr", cell_size=5.0, verbose=False)

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 2.385709228515625e02) < tol
    assert abs(min(mesh.points[:, 0]) - 3.307421875000000e01) < tol
    assert abs(max(mesh.points[:, 1]) - 1.947126770019531e02) < tol
    assert abs(min(mesh.points[:, 1]) - 2.314493751525879e01) < tol
    assert abs(max(mesh.points[:, 2]) - 1.957076873779297e02) < tol
    assert abs(min(mesh.points[:, 2]) - 1.365468883514404e01) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 1.759106599498153e06) < tol
    return
