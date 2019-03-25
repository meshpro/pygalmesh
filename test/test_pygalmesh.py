# -*- coding: utf-8 -*-
#
import numpy

import pygalmesh

import helpers


def test_ball():
    s = pygalmesh.Ball([0.0, 0.0, 0.0], 1.0)
    mesh = pygalmesh.generate_mesh(s, cell_size=0.2, verbose=False)

    assert abs(max(mesh.points[:, 0]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 0]) + 1.0) < 0.02
    assert abs(max(mesh.points[:, 1]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 1]) + 1.0) < 0.02
    assert abs(max(mesh.points[:, 2]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 2]) + 1.0) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 4.0 / 3.0 * numpy.pi) < 0.15
    return


def test_balls_union():
    radius = 1.0
    displacement = 0.5
    s0 = pygalmesh.Ball([displacement, 0, 0], radius)
    s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
    u = pygalmesh.Union([s0, s1])

    a = numpy.sqrt(radius ** 2 - displacement ** 2)
    edge_size = 0.1
    n = int(2 * numpy.pi * a / edge_size)
    circ = [
        [0.0, a * numpy.cos(i * 2 * numpy.pi / n), a * numpy.sin(i * 2 * numpy.pi / n)]
        for i in range(n)
    ]
    circ.append(circ[0])

    mesh = pygalmesh.generate_mesh(
        u, feature_edges=[circ], cell_size=0.15, edge_size=edge_size, verbose=False
    )

    assert abs(max(mesh.points[:, 0]) - (radius + displacement)) < 0.02
    assert abs(min(mesh.points[:, 0]) + (radius + displacement)) < 0.02
    assert abs(max(mesh.points[:, 1]) - radius) < 0.02
    assert abs(min(mesh.points[:, 1]) + radius) < 0.02
    assert abs(max(mesh.points[:, 2]) - radius) < 0.02
    assert abs(min(mesh.points[:, 2]) + radius) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    h = radius - displacement
    ref_vol = 2 * (
        4.0 / 3.0 * numpy.pi * radius ** 3 - h * numpy.pi / 6.0 * (3 * a ** 2 + h ** 2)
    )

    assert abs(vol - ref_vol) < 0.1

    return


def test_balls_intersection():
    radius = 1.0
    displacement = 0.5
    s0 = pygalmesh.Ball([displacement, 0, 0], radius)
    s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
    u = pygalmesh.Intersection([s0, s1])

    a = numpy.sqrt(radius ** 2 - displacement ** 2)
    edge_size = 0.1
    n = int(2 * numpy.pi * a / edge_size)
    circ = [
        [0.0, a * numpy.cos(i * 2 * numpy.pi / n), a * numpy.sin(i * 2 * numpy.pi / n)]
        for i in range(n)
    ]
    circ.append(circ[0])

    mesh = pygalmesh.generate_mesh(
        u, feature_edges=[circ], cell_size=0.15, edge_size=edge_size, verbose=False
    )

    assert abs(max(mesh.points[:, 0]) - (radius - displacement)) < 0.02
    assert abs(min(mesh.points[:, 0]) + (radius - displacement)) < 0.02
    assert abs(max(mesh.points[:, 1]) - a) < 0.02
    assert abs(min(mesh.points[:, 1]) + a) < 0.02
    assert abs(max(mesh.points[:, 2]) - a) < 0.02
    assert abs(min(mesh.points[:, 2]) + a) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    h = radius - displacement
    ref_vol = 2 * (h * numpy.pi / 6.0 * (3 * a ** 2 + h ** 2))

    assert abs(vol - ref_vol) < 0.1

    return


def test_balls_difference():
    radius = 1.0
    displacement = 0.5
    s0 = pygalmesh.Ball([displacement, 0, 0], radius)
    s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
    u = pygalmesh.Difference(s0, s1)

    a = numpy.sqrt(radius ** 2 - displacement ** 2)
    edge_size = 0.15
    n = int(2 * numpy.pi * a / edge_size)
    circ = [
        [0.0, a * numpy.cos(i * 2 * numpy.pi / n), a * numpy.sin(i * 2 * numpy.pi / n)]
        for i in range(n)
    ]
    circ.append(circ[0])

    mesh = pygalmesh.generate_mesh(
        u,
        feature_edges=[circ],
        cell_size=0.15,
        edge_size=edge_size,
        facet_angle=25,
        facet_size=0.15,
        cell_radius_edge_ratio=2.0,
        verbose=False,
    )

    tol = 0.02
    assert abs(max(mesh.points[:, 0]) - (radius + displacement)) < tol
    assert abs(min(mesh.points[:, 0]) - 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - radius) < tol
    assert abs(min(mesh.points[:, 1]) + radius) < tol
    assert abs(max(mesh.points[:, 2]) - radius) < tol
    assert abs(min(mesh.points[:, 2]) + radius) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    h = radius - displacement
    ref_vol = 4.0 / 3.0 * numpy.pi * radius ** 3 - 2 * h * numpy.pi / 6.0 * (
        3 * a ** 2 + h ** 2
    )

    assert abs(vol - ref_vol) < 0.05

    return


def test_cuboids_intersection():
    c0 = pygalmesh.Cuboid([0, 0, -0.5], [3, 3, 0.5])
    c1 = pygalmesh.Cuboid([1, 1, -2], [2, 2, 2])
    u = pygalmesh.Intersection([c0, c1])

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

    mesh = pygalmesh.generate_mesh(u, cell_size=0.1, edge_size=0.1, verbose=False)

    # filter the vertices that belong to cells
    verts = mesh.points[numpy.unique(mesh.cells["tetra"])]

    tol = 1.0e-2
    assert abs(max(verts[:, 0]) - 2.0) < tol
    assert abs(min(verts[:, 0]) - 1.0) < tol
    assert abs(max(verts[:, 1]) - 2.0) < tol
    assert abs(min(verts[:, 1]) - 1.0) < tol
    assert abs(max(verts[:, 2]) - 0.5) < 0.05
    assert abs(min(verts[:, 2]) + 0.5) < 0.05

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 1.0) < 0.05

    return


def test_cuboids_union():
    c0 = pygalmesh.Cuboid([0, 0, -0.5], [3, 3, 0.5])
    c1 = pygalmesh.Cuboid([1, 1, -2], [2, 2, 2])
    u = pygalmesh.Union([c0, c1])

    mesh = pygalmesh.generate_mesh(u, cell_size=0.2, edge_size=0.2, verbose=False)

    # filter the vertices that belong to cells
    verts = mesh.points[numpy.unique(mesh.cells["tetra"])]

    tol = 1.0e-2
    assert abs(max(verts[:, 0]) - 3.0) < tol
    assert abs(min(verts[:, 0]) - 0.0) < tol
    assert abs(max(verts[:, 1]) - 3.0) < tol
    assert abs(min(verts[:, 1]) - 0.0) < tol
    assert abs(max(verts[:, 2]) - 2.0) < tol
    assert abs(min(verts[:, 2]) + 2.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 12.0) < 0.1
    return


def test_cuboid():
    s0 = pygalmesh.Cuboid([0, 0, 0], [1, 2, 3])
    mesh = pygalmesh.generate_mesh(s0, edge_size=0.1, verbose=False)

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 1.0) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 6.0) < tol
    return


def test_cone():
    base_radius = 1.0
    height = 2.0
    edge_size = 0.1
    s0 = pygalmesh.Cone(base_radius, height, edge_size)
    mesh = pygalmesh.generate_mesh(
        s0, cell_size=0.1, edge_size=edge_size, verbose=False
    )

    tol = 2.0e-1
    assert abs(max(mesh.points[:, 0]) - base_radius) < tol
    assert abs(min(mesh.points[:, 0]) + base_radius) < tol
    assert abs(max(mesh.points[:, 1]) - base_radius) < tol
    assert abs(min(mesh.points[:, 1]) + base_radius) < tol
    assert abs(max(mesh.points[:, 2]) - height) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    ref_vol = numpy.pi * base_radius * base_radius / 3.0 * height
    assert abs(vol - ref_vol) < tol
    return


def test_cylinder():
    radius = 1.0
    z0 = 0.0
    z1 = 1.0
    edge_length = 0.1
    s0 = pygalmesh.Cylinder(z0, z1, radius, edge_length)
    mesh = pygalmesh.generate_mesh(
        s0, cell_size=0.1, edge_size=edge_length, verbose=False
    )

    tol = 1.0e-1
    assert abs(max(mesh.points[:, 0]) - radius) < tol
    assert abs(min(mesh.points[:, 0]) + radius) < tol
    assert abs(max(mesh.points[:, 1]) - radius) < tol
    assert abs(min(mesh.points[:, 1]) + radius) < tol
    assert abs(max(mesh.points[:, 2]) - z1) < tol
    assert abs(min(mesh.points[:, 2]) + z0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    ref_vol = numpy.pi * radius * radius * (z1 - z0)
    assert abs(vol - ref_vol) < tol
    return


def test_tetrahedron():
    s0 = pygalmesh.Tetrahedron(
        [0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]
    )
    mesh = pygalmesh.generate_mesh(s0, cell_size=0.1, edge_size=0.1, verbose=False)

    tol = 1.0e-4
    assert abs(max(mesh.points[:, 0]) - 1.0) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 1.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 1.0 / 6.0) < tol
    return


def test_torus():
    major_radius = 1.0
    minor_radius = 0.5
    s0 = pygalmesh.Torus(major_radius, minor_radius)
    mesh = pygalmesh.generate_mesh(s0, cell_size=0.1, verbose=False)

    tol = 1.0e-2
    radii_sum = major_radius + minor_radius
    assert abs(max(mesh.points[:, 0]) - radii_sum) < tol
    assert abs(min(mesh.points[:, 0]) + radii_sum) < tol
    assert abs(max(mesh.points[:, 1]) - radii_sum) < tol
    assert abs(min(mesh.points[:, 1]) + radii_sum) < tol
    assert abs(max(mesh.points[:, 2]) - minor_radius) < tol
    assert abs(min(mesh.points[:, 2]) + minor_radius) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    ref_vol = (numpy.pi * minor_radius * minor_radius) * (2 * numpy.pi * major_radius)
    assert abs(vol - ref_vol) < 1.0e-1
    return


def test_custom_function():
    class Hyperboloid(pygalmesh.DomainBase):
        def __init__(self, edge_size):
            super(Hyperboloid, self).__init__()
            self.z0 = -1.0
            self.z1 = 1.0
            self.waist_radius = 0.5
            self.edge_size = edge_size
            return

        def eval(self, x):
            if self.z0 < x[2] and x[2] < self.z1:
                return x[0] ** 2 + x[1] ** 2 - (x[2] ** 2 + self.waist_radius) ** 2
            return 1.0

        def get_bounding_sphere_squared_radius(self):
            z_max = max(abs(self.z0), abs(self.z1))
            r_max = z_max ** 2 + self.waist_radius
            return r_max * r_max + z_max * z_max

        def get_features(self):
            radius0 = self.z0 ** 2 + self.waist_radius
            n0 = int(2 * numpy.pi * radius0 / self.edge_size)
            circ0 = [
                [
                    radius0 * numpy.cos((2 * numpy.pi * k) / n0),
                    radius0 * numpy.sin((2 * numpy.pi * k) / n0),
                    self.z0,
                ]
                for k in range(n0)
            ]
            circ0.append(circ0[0])

            radius1 = self.z1 ** 2 + self.waist_radius
            n1 = int(2 * numpy.pi * radius1 / self.edge_size)
            circ1 = [
                [
                    radius1 * numpy.cos((2 * numpy.pi * k) / n1),
                    radius1 * numpy.sin((2 * numpy.pi * k) / n1),
                    self.z1,
                ]
                for k in range(n1)
            ]
            circ1.append(circ1[0])
            return [circ0, circ1]

    edge_size = 0.12
    d = Hyperboloid(edge_size)

    mesh = pygalmesh.generate_mesh(d, cell_size=0.1, edge_size=edge_size, verbose=False)

    # TODO check the reference values
    tol = 1.0e-1
    assert abs(max(mesh.points[:, 0]) - 1.4) < tol
    assert abs(min(mesh.points[:, 0]) + 1.4) < tol
    assert abs(max(mesh.points[:, 1]) - 1.4) < tol
    assert abs(min(mesh.points[:, 1]) + 1.4) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 1.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 2 * numpy.pi * 47.0 / 60.0) < 0.15
    return


def test_scaling():
    alpha = 1.3
    s = pygalmesh.Scale(pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), alpha)
    mesh = pygalmesh.generate_mesh(s, cell_size=0.2, edge_size=0.1, verbose=False)

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 1 * alpha) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2 * alpha) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3 * alpha) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 6.0 * alpha ** 3) < tol
    return


def test_stretch():
    alpha = 2.0
    s = pygalmesh.Stretch(pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), [alpha, 0.0, 0.0])
    mesh = pygalmesh.generate_mesh(s, cell_size=0.2, edge_size=0.2, verbose=False)

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - alpha) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 12.0) < tol

    return


def test_rotation():
    s0 = pygalmesh.Rotate(
        pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), [1.0, 0.0, 0.0], numpy.pi / 12.0
    )
    mesh = pygalmesh.generate_mesh(s0, cell_size=0.1, edge_size=0.1, verbose=False)

    tol = 1.0e-3
    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 6.0) < tol
    return


def test_translation():
    s0 = pygalmesh.Translate(pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), [1.0, 0.0, 0.0])
    mesh = pygalmesh.generate_mesh(s0, cell_size=0.1, edge_size=0.1, verbose=False)

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 2.0) < tol
    assert abs(min(mesh.points[:, 0]) - 1.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol
    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 6.0) < tol
    return


def test_off():
    helpers.download("elephant.vtu", "3552eb5b7f549b345d871999b4218dfc")
    mesh = pygalmesh.generate_volume_mesh_from_surface_mesh(
        "/tmp/elephant.vtu",
        facet_angle=25.0,
        facet_size=0.15,
        facet_distance=0.008,
        cell_radius_edge_ratio=3.0,
        verbose=False,
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 0.357612477657) < tol
    assert abs(min(mesh.points[:, 0]) + 0.358747130015) < tol
    assert abs(max(mesh.points[:, 1]) - 0.496137874959) < tol
    assert abs(min(mesh.points[:, 1]) + 0.495301051456) < tol
    assert abs(max(mesh.points[:, 2]) - 0.298780230629) < tol
    assert abs(min(mesh.points[:, 2]) + 0.300472866512) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 0.044164693065) < tol
    return


def test_extrude():
    p = pygalmesh.Polygon2D([[-0.5, -0.3], [0.5, -0.3], [0.0, 0.5]])
    domain = pygalmesh.Extrude(p, [0.0, 0.3, 1.0])
    mesh = pygalmesh.generate_mesh(domain, cell_size=0.1, edge_size=0.1, verbose=False)

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 0.5) < tol
    assert abs(min(mesh.points[:, 0]) + 0.5) < tol
    assert abs(max(mesh.points[:, 1]) - 0.8) < tol
    assert abs(min(mesh.points[:, 1]) + 0.3) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 0.4) < tol
    return


def test_extrude_rotate():
    p = pygalmesh.Polygon2D([[-0.5, -0.3], [0.5, -0.3], [0.0, 0.5]])
    edge_size = 0.1
    domain = pygalmesh.Extrude(p, [0.0, 0.0, 1.0], 0.5 * 3.14159265359, edge_size)
    mesh = pygalmesh.generate_mesh(
        domain, cell_size=0.1, edge_size=edge_size, verbose=False
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 0.583012701892) < tol
    assert abs(min(mesh.points[:, 0]) + 0.5) < tol
    assert abs(max(mesh.points[:, 1]) - 0.5) < tol
    assert abs(min(mesh.points[:, 1]) + 0.583012701892) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 0.4) < 0.05
    return


def test_ring_extrude():
    p = pygalmesh.Polygon2D([[0.5, -0.3], [1.5, -0.3], [1.0, 0.5]])
    edge_size = 0.1
    domain = pygalmesh.RingExtrude(p, edge_size)
    mesh = pygalmesh.generate_mesh(
        domain, cell_size=0.1, edge_size=edge_size, verbose=False
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 1.5) < tol
    assert abs(min(mesh.points[:, 0]) + 1.5) < tol
    assert abs(max(mesh.points[:, 1]) - 1.5) < tol
    assert abs(min(mesh.points[:, 1]) + 1.5) < tol
    assert abs(max(mesh.points[:, 2]) - 0.5) < tol
    assert abs(min(mesh.points[:, 2]) + 0.3) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 2 * numpy.pi * 0.4) < 0.05
    return


# def test_heart():
#     class Heart(pygalmesh.DomainBase):
#         def __init__(self, edge_size):
#             super(Heart, self).__init__()
#             return
#
#         def eval(self, x):
#             return (x[0]**2 + 9.0/4.0 * x[1]**2 + x[2]**2 - 1)**3 \
#                 - x[0]**2 * x[2]**3 - 9.0/80.0 * x[1]**2 * x[2]**3
#
#         def get_bounding_sphere_squared_radius(self):
#             return 10.0
#
#     edge_size = 0.1
#     d = Heart(edge_size)
#
#     pygalmesh.generate_mesh(
#             d,
#             'out.mesh',
#             cell_size=0.1,
#             edge_size=edge_size,
#             # odt=True,
#             # lloyd=True,
#             # verbose=True
#             )
#
#     return


def test_sphere():
    radius = 1.0
    s = pygalmesh.Ball([0.0, 0.0, 0.0], radius)
    mesh = pygalmesh.generate_surface_mesh(
        s, angle_bound=30, radius_bound=0.1, distance_bound=0.1, verbose=False
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - radius) < tol
    assert abs(min(mesh.points[:, 0]) + radius) < tol
    assert abs(max(mesh.points[:, 1]) - radius) < tol
    assert abs(min(mesh.points[:, 1]) + radius) < tol
    assert abs(max(mesh.points[:, 2]) - radius) < tol
    assert abs(min(mesh.points[:, 2]) + radius) < tol

    areas = helpers.compute_triangle_areas(mesh.points, mesh.cells["triangle"])
    surface_area = sum(areas)
    assert abs(surface_area - 4 * numpy.pi * radius ** 2) < 0.1
    return


def test_halfspace():
    c = pygalmesh.Cuboid([0, 0, 0], [1, 1, 1])
    s = pygalmesh.HalfSpace([1.0, 2.0, 3.0], 1.0, 2.0)
    u = pygalmesh.Intersection([c, s])

    mesh = pygalmesh.generate_mesh(u, cell_size=0.2, edge_size=0.2, verbose=False)

    vol = sum(helpers.compute_volumes(mesh.points, mesh.cells["tetra"]))
    assert abs(vol - 1 / 750) < 1.0e-3
    return


if __name__ == "__main__":
    test_sphere()
