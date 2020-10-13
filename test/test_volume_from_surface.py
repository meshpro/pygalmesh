import pathlib
import tempfile

import helpers
import meshio

import pygalmesh


def test_volume_from_surface():
    this_dir = pathlib.Path(__file__).resolve().parent
    # mesh = pygalmesh.generate_volume_mesh_from_surface_mesh(
    #     this_dir / "meshes" / "elephant.vtu",
    #     min_facet_angle=25.0,
    #     max_radius_surface_delaunay_ball=0.15,
    #     max_facet_distance=0.008,
    #     max_circumradius_edge_ratio=3.0,
    #     verbose=False,
    # )
    with tempfile.TemporaryDirectory() as tmp:
        out_filename = str(pathlib.Path(tmp) / "out.vtk")
        pygalmesh._cli.volume_from_surface(
            [
                str(this_dir / "meshes" / "elephant.vtu"),
                out_filename,
                "--min-facet-angle",
                "0.5",
                "--max-radius-surface-delaunay-ball",
                "0.15",
                "--max-facet-distance",
                "0.008",
                "--max-circumradius-edge-ratio",
                "3.0",
                "--quiet",
            ]
        )
        mesh = meshio.read(out_filename)

    tol = 2.0e-2
    assert abs(max(mesh.points[:, 0]) - 0.357612477657) < tol
    assert abs(min(mesh.points[:, 0]) + 0.358747130015) < tol
    assert abs(max(mesh.points[:, 1]) - 0.496137874959) < tol
    assert abs(min(mesh.points[:, 1]) + 0.495301051456) < tol
    assert abs(max(mesh.points[:, 2]) - 0.298780230629) < tol
    assert abs(min(mesh.points[:, 2]) + 0.300472866512) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 0.044164693065) < tol
