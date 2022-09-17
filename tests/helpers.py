import numpy as np


def _row_dot(a, b):
    # http://stackoverflow.com/a/26168677/353337
    return np.einsum("ij, ij->i", a, b)


def compute_volumes(vertices, tets):
    cell_coords = vertices[tets]

    a = cell_coords[:, 1, :] - cell_coords[:, 0, :]
    b = cell_coords[:, 2, :] - cell_coords[:, 0, :]
    c = cell_coords[:, 3, :] - cell_coords[:, 0, :]

    # omega = <a, b x c>
    omega = _row_dot(a, np.cross(b, c))

    # https://en.wikipedia.org/wiki/Tetrahedron#Volume
    return abs(omega) / 6.0


def compute_triangle_areas(vertices, triangles):
    e0 = vertices[triangles[:, 1]] - vertices[triangles[:, 0]]
    e1 = vertices[triangles[:, 2]] - vertices[triangles[:, 1]]

    assert e0.shape == e1.shape
    if e0.shape[1] == 2:
        z_component_of_e0_cross_e1 = np.cross(e0, e1)
        cross_magnitude = z_component_of_e0_cross_e1
    else:
        assert e0.shape[1] == 3
        e0_cross_e1 = np.cross(e0, e1)
        cross_magnitude = np.sqrt(_row_dot(e0_cross_e1, e0_cross_e1))

    return 0.5 * cross_magnitude
