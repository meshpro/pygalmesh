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

    vals_refs = [
        (max(mesh.points[:, 0]), 9.00478554e00),
        (min(mesh.points[:, 0]), -4.25843196e-03),
        (max(mesh.points[:, 1]), 9.00332642e00),
        (min(mesh.points[:, 1]), -4.41271299e-03),
        (max(mesh.points[:, 2]), 9.00407982e00),
        (min(mesh.points[:, 2]), -3.98639357e-03),
    ]
    for val, ref in vals_refs:
        assert abs(val - ref) < 1.0e-3 * (1.0 + abs(ref)), f"{val:.8e} != {ref:.8e}"

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 6.95558790e02
    # Debian needs 2.0e-2 here.
    # <https://github.com/nschloe/pygalmesh/issues/60>
    assert abs(vol - ref) < ref * 2.0e-2, f"{vol:.8e}"
