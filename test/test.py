# -*- coding: utf-8 -*-
#
import loom

import numpy
import meshio


def compute_volumes(vertices, tets):
    cell_coords = vertices[tets]

    a = cell_coords[:, 1, :] - cell_coords[:, 0, :]
    b = cell_coords[:, 2, :] - cell_coords[:, 0, :]
    c = cell_coords[:, 3, :] - cell_coords[:, 0, :]

    # omega = <a, b x c>
    omega = numpy.einsum('ij, ij->i', a, numpy.cross(b, c))

    # https://en.wikipedia.org/wiki/Tetrahedron#Volume
    return abs(omega) / 6.0


def test_ball():
    s = loom.Ball([0, 0, 0], 1.0)
    loom.generate_mesh(s, 'out.mesh', cell_size=0.2, verbose=False)

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    assert abs(max(vertices[:, 0]) - 1.0) < 0.02
    assert abs(min(vertices[:, 0]) + 1.0) < 0.02
    assert abs(max(vertices[:, 1]) - 1.0) < 0.02
    assert abs(min(vertices[:, 1]) + 1.0) < 0.02
    assert abs(max(vertices[:, 2]) - 1.0) < 0.02
    assert abs(min(vertices[:, 2]) + 1.0) < 0.02

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 4.0/3.0 * numpy.pi) < 0.15

    return


def test_balls_union():
    radius = 1.0
    displacement = 0.5
    s0 = loom.Ball([displacement, 0, 0], radius)
    s1 = loom.Ball([-displacement, 0, 0], radius)
    uni = loom.ListOfDomains()
    uni.append(s0)
    uni.append(s1)
    u = loom.Union(uni)

    a = numpy.sqrt(radius**2 - displacement**2)
    edge_size = 0.1
    n = int(2*numpy.pi*a / edge_size)
    circ = [
        [
            0.0,
            a * numpy.cos(i * 2*numpy.pi / n),
            a * numpy.sin(i * 2*numpy.pi / n)
        ] for i in range(n)
        ]
    circ.append(circ[0])

    loom.generate_mesh(
            u,
            'out.mesh',
            feature_edges=[circ],
            cell_size=0.15,
            edge_size=edge_size,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    assert abs(max(vertices[:, 0]) - (radius + displacement)) < 0.02
    assert abs(min(vertices[:, 0]) + (radius + displacement)) < 0.02
    assert abs(max(vertices[:, 1]) - radius) < 0.02
    assert abs(min(vertices[:, 1]) + radius) < 0.02
    assert abs(max(vertices[:, 2]) - radius) < 0.02
    assert abs(min(vertices[:, 2]) + radius) < 0.02

    vol = sum(compute_volumes(vertices, cells['tetra']))
    h = radius - displacement
    ref_vol = 2 * (
        4.0/3.0 * numpy.pi * radius**3
        - h * numpy.pi / 6.0 * (3*a**2 + h**2)
        )

    assert abs(vol - ref_vol) < 0.1

    return


def test_balls_intersection():
    radius = 1.0
    displacement = 0.5
    s0 = loom.Ball([displacement, 0, 0], radius)
    s1 = loom.Ball([-displacement, 0, 0], radius)
    inter = loom.ListOfDomains()
    inter.append(s0)
    inter.append(s1)
    u = loom.Intersection(inter)

    a = numpy.sqrt(radius**2 - displacement**2)
    edge_size = 0.1
    n = int(2*numpy.pi*a / edge_size)
    circ = [
        [
            0.0,
            a * numpy.cos(i * 2*numpy.pi / n),
            a * numpy.sin(i * 2*numpy.pi / n)
        ] for i in range(n)
        ]
    circ.append(circ[0])

    loom.generate_mesh(
            u,
            'out.mesh',
            feature_edges=[circ],
            cell_size=0.15,
            edge_size=edge_size,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    assert abs(max(vertices[:, 0]) - (radius - displacement)) < 0.02
    assert abs(min(vertices[:, 0]) + (radius - displacement)) < 0.02
    assert abs(max(vertices[:, 1]) - a) < 0.02
    assert abs(min(vertices[:, 1]) + a) < 0.02
    assert abs(max(vertices[:, 2]) - a) < 0.02
    assert abs(min(vertices[:, 2]) + a) < 0.02

    vol = sum(compute_volumes(vertices, cells['tetra']))
    h = radius - displacement
    ref_vol = 2 * (
        h * numpy.pi / 6.0 * (3*a**2 + h**2)
        )

    assert abs(vol - ref_vol) < 0.1

    return


def test_balls_difference():
    radius = 1.0
    displacement = 0.5
    s0 = loom.Ball([displacement, 0, 0], radius)
    s1 = loom.Ball([-displacement, 0, 0], radius)
    u = loom.Difference(s0, s1)

    a = numpy.sqrt(radius**2 - displacement**2)
    edge_size = 0.15
    n = int(2*numpy.pi*a / edge_size)
    circ = [
        [
            0.0,
            a * numpy.cos(i * 2*numpy.pi / n),
            a * numpy.sin(i * 2*numpy.pi / n)
        ] for i in range(n)
        ]
    circ.append(circ[0])

    loom.generate_mesh(
            u,
            'out.mesh',
            feature_edges=[circ],
            cell_size=0.15,
            edge_size=edge_size,
            facet_angle=25,
            facet_size=0.15,
            cell_radius_edge_ratio=2.0,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 0.02
    assert abs(max(vertices[:, 0]) - (radius + displacement)) < tol
    assert abs(min(vertices[:, 0]) - 0.0) < tol
    assert abs(max(vertices[:, 1]) - radius) < tol
    assert abs(min(vertices[:, 1]) + radius) < tol
    assert abs(max(vertices[:, 2]) - radius) < tol
    assert abs(min(vertices[:, 2]) + radius) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    h = radius - displacement
    ref_vol = 4.0/3.0 * numpy.pi * radius**3 \
        - 2 * h * numpy.pi / 6.0 * (3*a**2 + h**2)

    assert abs(vol - ref_vol) < 0.05

    return


def test_cuboids_intersection():
    c0 = loom.Cuboid([0, 0, -0.5], [3, 3, 0.5])
    c1 = loom.Cuboid([1, 1, -2], [2, 2, 2])
    inter = loom.ListOfDomains()
    inter.append(c0)
    inter.append(c1)
    u = loom.Intersection(inter)

    # In CGAL, feature edges must not intersect, and that's a problem here: The
    # intersection edges of the cuboids share eight points with the edges of
    # the tall and skinny cuboid.
    # eps = 1.0e-2
    # extra_features = [
    #         [[1.0, 1.0 + eps, 0.5], [1.0, 2.0 - eps, 0.5]],
    #         [[1.0 + eps, 2.0, 0.5], [2.0 - eps, 2.0, 0.5]],
    #         [[2.0, 2.0 - eps, 0.5], [2.0, 1.0 + eps, 0.5]],
    #         [[2.0 - eps, 1.0, 0.5], [1.0 + eps, 1.0, 0.5]],
    #         ]

    loom.generate_mesh(
            u,
            'out.mesh',
            cell_size=0.1,
            edge_size=0.1,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    # filter the vertices that belong to cells
    verts = vertices[numpy.unique(cells['tetra'])]

    tol = 1.0e-2
    assert abs(max(verts[:, 0]) - 2.0) < tol
    assert abs(min(verts[:, 0]) - 1.0) < tol
    assert abs(max(verts[:, 1]) - 2.0) < tol
    assert abs(min(verts[:, 1]) - 1.0) < tol
    assert abs(max(verts[:, 2]) - 0.5) < 0.05
    assert abs(min(verts[:, 2]) + 0.5) < 0.05

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 1.0) < 0.05

    return


def test_cuboids_union():
    c0 = loom.Cuboid([0, 0, -0.5], [3, 3, 0.5])
    c1 = loom.Cuboid([1, 1, -2], [2, 2, 2])
    inter = loom.ListOfDomains()
    inter.append(c0)
    inter.append(c1)
    u = loom.Union(inter)

    loom.generate_mesh(
            u,
            'out.mesh',
            cell_size=0.2,
            edge_size=0.2,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    # filter the vertices that belong to cells
    verts = vertices[numpy.unique(cells['tetra'])]

    tol = 1.0e-2
    assert abs(max(verts[:, 0]) - 3.0) < tol
    assert abs(min(verts[:, 0]) - 0.0) < tol
    assert abs(max(verts[:, 1]) - 3.0) < tol
    assert abs(min(verts[:, 1]) - 0.0) < tol
    assert abs(max(verts[:, 2]) - 2.0) < tol
    assert abs(min(verts[:, 2]) + 2.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 12.0) < 0.1

    return


def test_cuboid():
    s0 = loom.Cuboid([0, 0, 0], [1, 2, 3])
    loom.generate_mesh(s0, 'out.mesh', edge_size=0.1, verbose=False)

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-3
    assert abs(max(vertices[:, 0]) - 1.0) < tol
    assert abs(min(vertices[:, 0]) + 0.0) < tol
    assert abs(max(vertices[:, 1]) - 2.0) < tol
    assert abs(min(vertices[:, 1]) + 0.0) < tol
    assert abs(max(vertices[:, 2]) - 3.0) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 6.0) < tol

    return


def test_cone():
    base_radius = 1.0
    height = 2.0
    edge_size = 0.1
    s0 = loom.Cone(base_radius, height, edge_size)
    loom.generate_mesh(
            s0, 'out.mesh', cell_size=0.1, edge_size=edge_size,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 2.0e-1
    assert abs(max(vertices[:, 0]) - base_radius) < tol
    assert abs(min(vertices[:, 0]) + base_radius) < tol
    assert abs(max(vertices[:, 1]) - base_radius) < tol
    assert abs(min(vertices[:, 1]) + base_radius) < tol
    assert abs(max(vertices[:, 2]) - height) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    ref_vol = numpy.pi*base_radius*base_radius / 3.0 * height
    assert abs(vol - ref_vol) < tol

    return


def test_cylinder():
    radius = 1.0
    z0 = 0.0
    z1 = 1.0
    edge_length = 0.1
    s0 = loom.Cylinder(z0, z1, radius, edge_length)
    loom.generate_mesh(
            s0, 'out.mesh', cell_size=0.1, edge_size=edge_length,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-1
    assert abs(max(vertices[:, 0]) - radius) < tol
    assert abs(min(vertices[:, 0]) + radius) < tol
    assert abs(max(vertices[:, 1]) - radius) < tol
    assert abs(min(vertices[:, 1]) + radius) < tol
    assert abs(max(vertices[:, 2]) - z1) < tol
    assert abs(min(vertices[:, 2]) + z0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    ref_vol = numpy.pi*radius*radius * (z1 - z0)
    assert abs(vol - ref_vol) < tol

    return


def test_tetrahedron():
    s0 = loom.Tetrahedron(
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0]
            )
    loom.generate_mesh(
            s0, 'out.mesh', cell_size=0.1, edge_size=0.1,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-4
    assert abs(max(vertices[:, 0]) - 1.0) < tol
    assert abs(min(vertices[:, 0]) + 0.0) < tol
    assert abs(max(vertices[:, 1]) - 1.0) < tol
    assert abs(min(vertices[:, 1]) + 0.0) < tol
    assert abs(max(vertices[:, 2]) - 1.0) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 1.0/6.0) < tol

    return


def test_torus():
    major_radius = 1.0
    minor_radius = 0.5
    s0 = loom.Torus(major_radius, minor_radius)
    loom.generate_mesh(s0, 'out.mesh', cell_size=0.1, verbose=False)

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-2
    radii_sum = major_radius + minor_radius
    assert abs(max(vertices[:, 0]) - radii_sum) < tol
    assert abs(min(vertices[:, 0]) + radii_sum) < tol
    assert abs(max(vertices[:, 1]) - radii_sum) < tol
    assert abs(min(vertices[:, 1]) + radii_sum) < tol
    assert abs(max(vertices[:, 2]) - minor_radius) < tol
    assert abs(min(vertices[:, 2]) + minor_radius) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    ref_vol = (numpy.pi * minor_radius * minor_radius) * \
        (2 * numpy.pi * major_radius)
    assert abs(vol - ref_vol) < 1.0e-1

    return


# def test_custom_function():
#     class Hyperboloid(loom.DomainBase):
#         def __init__(self, edge_size):
#             super(Hyperboloid, self).__init__()
#             self.z0 = -1.0
#             self.z1 = 1.0
#             self.waist_radius = 0.5
#             self.edge_size = edge_size
#             return
#
#         def eval(self, x):
#             if self.z0 < x[2] and x[2] < self.z1:
#                 r2 = x[0]*x[0] + y[1]*y[1]
#                 return r2 - z[2]*z[2] - self.waist_radius*self.waist_radius
#             else:
#                 return 1.0
#
#         def get_bounding_sphere_squared_radius(self):
#             z_max = max(abs(self.z0), abs(self.z1))
#             r_max = z_max*z_max + self.waist_radius*self.waist_radius
#             return r_max*r_max + z_max*z_max
#
#         def get_features(self):
#             radius0 = numpy.sqrt(
#                 self.z0*self.z0 + self.waist_radius*self.waist_radius
#                 )
#             n0 = int(2*numpy.pi*radius0 / self.edge_size)
#             circ0 = [[
#                     radius0 * numpy.cos((2*numpy.pi * k) / n0),
#                     radius0 * numpy.sin((2*numpy.pi * k) / n0),
#                     self.z0
#                     ] for k in range(n0)
#                     ]
#             circ0.append(circ0[0])
#
#             radius1 = numpy.sqrt(
#                 self.z1*self.z1 + self.waist_radius*self.waist_radius
#                 )
#             n1 = int(2*numpy.pi*radius1 / self.edge_size)
#             circ1 = [[
#                     radius1 * numpy.cos((2*numpy.pi * k) / n1),
#                     radius1 * numpy.sin((2*numpy.pi * k) / n1),
#                     self.z1
#                     ] for k in range(n1)
#                     ]
#             circ1.append(circ1[0])
#             return [circ0, circ1]
#
#     edge_size = 0.1
#     d = Hyperboloid(edge_size)
#
#     loom.generate_mesh(
#             d,
#             'out.mesh',
#             cell_size=0.1,
#             edge_size=edge_size,
#             verbose=False
#             )
#
#     return


def test_scaling():
    alpha = 1.3
    s = loom.Scale(loom.Cuboid([0, 0, 0], [1, 2, 3]), alpha)
    loom.generate_mesh(
            s,
            'out.mesh',
            cell_size=0.2,
            edge_size=0.1,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-3
    assert abs(max(vertices[:, 0]) - 1*alpha) < tol
    assert abs(min(vertices[:, 0]) + 0.0) < tol
    assert abs(max(vertices[:, 1]) - 2*alpha) < tol
    assert abs(min(vertices[:, 1]) + 0.0) < tol
    assert abs(max(vertices[:, 2]) - 3*alpha) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 6.0 * alpha**3) < tol

    return


def test_stretch():
    alpha = 2.0
    s = loom.Stretch(
            loom.Cuboid([0, 0, 0], [1, 2, 3]),
            [alpha, 0.0, 0.0]
            )
    loom.generate_mesh(
            s,
            'out.mesh',
            cell_size=0.2,
            edge_size=0.2,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-3
    assert abs(max(vertices[:, 0]) - alpha) < tol
    assert abs(min(vertices[:, 0]) + 0.0) < tol
    assert abs(max(vertices[:, 1]) - 2.0) < tol
    assert abs(min(vertices[:, 1]) + 0.0) < tol
    assert abs(max(vertices[:, 2]) - 3.0) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 12.0) < tol

    return


def test_rotation():
    s0 = loom.Rotate(
            loom.Cuboid([0, 0, 0], [1, 2, 3]),
            [1.0, 0.0, 0.0],
            numpy.pi / 12.0
            )
    loom.generate_mesh(
            s0,
            'out.mesh',
            cell_size=0.1,
            edge_size=0.1,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-3
    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 6.0) < tol

    return


def test_translation():
    s0 = loom.Translate(
            loom.Cuboid([0, 0, 0], [1, 2, 3]),
            [1.0, 0.0, 0.0]
            )
    loom.generate_mesh(
            s0,
            'out.mesh',
            cell_size=0.1,
            edge_size=0.1,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-3
    assert abs(max(vertices[:, 0]) - 2.0) < tol
    assert abs(min(vertices[:, 0]) - 1.0) < tol
    assert abs(max(vertices[:, 1]) - 2.0) < tol
    assert abs(min(vertices[:, 1]) + 0.0) < tol
    assert abs(max(vertices[:, 2]) - 3.0) < tol
    assert abs(min(vertices[:, 2]) + 0.0) < tol
    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 6.0) < tol

    return

def test_off():
    loom.generate_poly(
            'elephant.off',
            'out.mesh',
            facet_angle=25.0,
            facet_size=0.15,
            facet_distance=0.008,
            cell_radius_edge_ratio=3.0,
            verbose=False
            )

    vertices, cells, _, _, _ = meshio.read('out.mesh')

    tol = 1.0e-3
    assert abs(max(vertices[:, 0]) - 0.357612477657) < tol
    assert abs(min(vertices[:, 0]) + 0.358747130015) < tol
    assert abs(max(vertices[:, 1]) - 0.496137874959) < tol
    assert abs(min(vertices[:, 1]) + 0.495301051456) < tol
    assert abs(max(vertices[:, 2]) - 0.298780230629) < tol
    assert abs(min(vertices[:, 2]) + 0.300472866512) < tol

    vol = sum(compute_volumes(vertices, cells['tetra']))
    assert abs(vol - 0.044164693065) < tol

    return


if __name__ == '__main__':
    test_off()
