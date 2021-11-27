import pygalmesh


def test_cylinder():
    c = pygalmesh.Cylinder(-1.0, 1.0, 0.7, 0.4)
    mesh = pygalmesh.generate_mesh(
        c, max_cell_circumradius=0.4, max_edge_size_at_feature_edges=0.4, verbose=False
    )
    assert len(mesh.points) == 71
    assert mesh.cells[0].type == "triangle"
    assert len(mesh.cells[0].data) == 220
