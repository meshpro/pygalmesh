import numpy


def _row_dot(a, b):
    # http://stackoverflow.com/a/26168677/353337
    return numpy.einsum("ij, ij->i", a, b)


def compute_volumes(vertices, tets):
    cell_coords = vertices[tets]

    a = cell_coords[:, 1, :] - cell_coords[:, 0, :]
    b = cell_coords[:, 2, :] - cell_coords[:, 0, :]
    c = cell_coords[:, 3, :] - cell_coords[:, 0, :]

    # omega = <a, b x c>
    omega = _row_dot(a, numpy.cross(b, c))

    # https://en.wikipedia.org/wiki/Tetrahedron#Volume
    return abs(omega) / 6.0


def compute_triangle_areas(vertices, triangles):
    e0 = vertices[triangles[:, 1]] - vertices[triangles[:, 0]]
    e1 = vertices[triangles[:, 2]] - vertices[triangles[:, 1]]

    e0_cross_e1 = numpy.cross(e0, e1)
    areas = 0.5 * numpy.sqrt(_row_dot(e0_cross_e1, e0_cross_e1))

    return areas
