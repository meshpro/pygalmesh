import pathlib
import tempfile

import helpers
import meshio

import pygalmesh


def test_remesh_surface():
    this_dir = pathlib.Path(__file__).resolve().parent
    # mesh = pygalmesh.remesh_surface(
    #     this_dir / "meshes" / "lion-head.vtu",
    #     max_edge_size_at_feature_edges=0.025,
    #     min_facet_angle=25,
    #     max_radius_surface_delaunay_ball=0.1,
    #     max_facet_distance=0.001,
    #     verbose=False,
    # )
    with tempfile.TemporaryDirectory() as tmp:
        out_filename = str(pathlib.Path(tmp) / "out.vtk")
        pygalmesh._cli.cli(
            [
                "remesh-surface",
                str(this_dir / "meshes" / "elephant.vtu"),
                out_filename,
                "--max-edge-size-at-feature-edges",
                "0.025",
                "--min-facet-angle",
                "25",
                "--max-radius-surface-delaunay-ball",
                "0.1",
                "--max-facet-distance",
                "0.001",
                "--quiet",
            ]
        )
        mesh = meshio.read(out_filename)

    vals_refs = [
        (max(mesh.points[:, 0]), 3.60217000e-01),
        (min(mesh.points[:, 0]), -3.60140000e-01),
        (max(mesh.points[:, 1]), 4.98948000e-01),
        (min(mesh.points[:, 1]), -4.99336000e-01),
        (max(mesh.points[:, 2]), 3.00977000e-01),
        (min(mesh.points[:, 2]), -3.01316000e-01),
    ]
    for val, ref in vals_refs:
        assert abs(val - ref) < 1.0e-3 * (1.0 + abs(ref)), f"{val:.8e} != {ref:.8e}"

    triangle_areas = helpers.compute_triangle_areas(
        mesh.points, mesh.get_cells_type("triangle")
    )
    vol = sum(triangle_areas)
    ref = 1.2357989593759846
    assert abs(vol - ref) < ref * 1.0e-3, vol
