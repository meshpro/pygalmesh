import pathlib
import tempfile

import helpers
import meshio

import pygalmesh


def test_inr():
    this_dir = pathlib.Path(__file__).resolve().parent
    # mesh = pygalmesh.generate_from_inr(
    #     this_dir / "meshes" / "skull_2.9.inr", max_cell_circumradius=5.0, verbose=False
    # )
    with tempfile.TemporaryDirectory() as tmp:
        out_filename = str(pathlib.Path(tmp) / "out.vtk")
        pygalmesh._cli.inr(
            [
                str(this_dir / "meshes" / "skull_2.9.inr"),
                out_filename,
                "--max-cell-circumradius",
                "5.0",
                "--quiet",
            ]
        )
        mesh = meshio.read(out_filename)

    tol = 2.0e-3
    ref = [2.031053e02, 3.739508e01, 2.425594e02, 2.558910e01, 2.300883e02, 1.775010e00]
    assert abs(max(mesh.points[:, 0]) - ref[0]) < tol * ref[0]
    assert abs(min(mesh.points[:, 0]) - ref[1]) < tol * ref[1]
    assert abs(max(mesh.points[:, 1]) - ref[2]) < tol * ref[2]
    assert abs(min(mesh.points[:, 1]) - ref[3]) < tol * ref[3]
    assert abs(max(mesh.points[:, 2]) - ref[4]) < tol * ref[4]
    tol = 3.0e-2
    assert abs(min(mesh.points[:, 2]) - ref[5]) < tol * ref[5]

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 2.725335e06
    # Debian needs 2.0e-2 here.
    # <https://github.com/nschloe/pygalmesh/issues/60>
    assert abs(vol - ref) < ref * 2.0e-2
