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
        pygalmesh._cli.cli(
            [
                "from-inr",
                str(this_dir / "meshes" / "sphere.inr"),
                out_filename,
                "--max-cell-circumradius",
                "1.0",
                "--quiet",
            ]
        )
        mesh = meshio.read(out_filename)

    tol = 2.0e-3
    refs = [
        9.00478554e00,
        -4.25843196e-03,
        9.00332642e00,
        -4.41271299e-03,
        9.00407982e00,
        -3.98639357e-03,
    ]
    vals = [
        max(mesh.points[:, 0]),
        min(mesh.points[:, 0]),
        max(mesh.points[:, 1]),
        min(mesh.points[:, 1]),
        max(mesh.points[:, 2]),
        min(mesh.points[:, 2]),
    ]
    for ref, val in zip(refs, vals):
        assert abs(val - ref) < tol * abs(ref), f"{val:.8e} != {ref:.8e}"

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 6.95558790e02
    # Debian needs 2.0e-2 here.
    # <https://github.com/nschloe/pygalmesh/issues/60>
    assert abs(vol - ref) < ref * 2.0e-2, f"{vol:.8e}"
