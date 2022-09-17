import helpers
import numpy as np

import pygalmesh


def test_ball():
    s = pygalmesh.Ball([0.0, 0.0, 0.0], 1.0)
    mesh = pygalmesh.generate_mesh(s, max_cell_circumradius=0.2, verbose=False)

    assert abs(max(mesh.points[:, 0]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 0]) + 1.0) < 0.02
    assert abs(max(mesh.points[:, 1]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 1]) + 1.0) < 0.02
    assert abs(max(mesh.points[:, 2]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 2]) + 1.0) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 4.0 / 3.0 * np.pi) < 0.15


def test_balls_union():
    radius = 1.0
    displacement = 0.5
    s0 = pygalmesh.Ball([displacement, 0, 0], radius)
    s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
    u = pygalmesh.Union([s0, s1])

    a = np.sqrt(radius**2 - displacement**2)
    max_edge_size_at_feature_edges = 0.1
    n = int(2 * np.pi * a / max_edge_size_at_feature_edges)
    circ = [
        [0.0, a * np.cos(i * 2 * np.pi / n), a * np.sin(i * 2 * np.pi / n)]
        for i in range(n)
    ]
    circ.append(circ[0])

    mesh = pygalmesh.generate_mesh(
        u,
        extra_feature_edges=[circ],
        max_cell_circumradius=0.15,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        verbose=False,
    )

    assert abs(max(mesh.points[:, 0]) - (radius + displacement)) < 0.02
    assert abs(min(mesh.points[:, 0]) + (radius + displacement)) < 0.02
    assert abs(max(mesh.points[:, 1]) - radius) < 0.02
    assert abs(min(mesh.points[:, 1]) + radius) < 0.02
    assert abs(max(mesh.points[:, 2]) - radius) < 0.02
    assert abs(min(mesh.points[:, 2]) + radius) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    h = radius - displacement
    ref_vol = 2 * (
        4.0 / 3.0 * np.pi * radius**3 - h * np.pi / 6.0 * (3 * a**2 + h**2)
    )

    assert abs(vol - ref_vol) < 0.1


def test_balls_intersection():
    radius = 1.0
    displacement = 0.5
    s0 = pygalmesh.Ball([displacement, 0, 0], radius)
    s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
    u = pygalmesh.Intersection([s0, s1])

    a = np.sqrt(radius**2 - displacement**2)
    max_edge_size_at_feature_edges = 0.1
    n = int(2 * np.pi * a / max_edge_size_at_feature_edges)
    circ = [
        [0.0, a * np.cos(i * 2 * np.pi / n), a * np.sin(i * 2 * np.pi / n)]
        for i in range(n)
    ]
    circ.append(circ[0])

    mesh = pygalmesh.generate_mesh(
        u,
        extra_feature_edges=[circ],
        max_cell_circumradius=0.15,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        verbose=False,
    )

    assert abs(max(mesh.points[:, 0]) - (radius - displacement)) < 0.02
    assert abs(min(mesh.points[:, 0]) + (radius - displacement)) < 0.02
    assert abs(max(mesh.points[:, 1]) - a) < 0.02
    assert abs(min(mesh.points[:, 1]) + a) < 0.02
    assert abs(max(mesh.points[:, 2]) - a) < 0.02
    assert abs(min(mesh.points[:, 2]) + a) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    h = radius - displacement
    ref_vol = 2 * (h * np.pi / 6.0 * (3 * a**2 + h**2))

    assert abs(vol - ref_vol) < 0.1


def test_balls_difference():
    radius = 1.0
    displacement = 0.5
    s0 = pygalmesh.Ball([displacement, 0, 0], radius)
    s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
    u = pygalmesh.Difference(s0, s1)

    a = np.sqrt(radius**2 - displacement**2)
    max_edge_size_at_feature_edges = 0.15
    n = int(2 * np.pi * a / max_edge_size_at_feature_edges)
    circ = [
        [0.0, a * np.cos(i * 2 * np.pi / n), a * np.sin(i * 2 * np.pi / n)]
        for i in range(n)
    ]
    circ.append(circ[0])

    mesh = pygalmesh.generate_mesh(
        u,
        extra_feature_edges=[circ],
        max_cell_circumradius=0.15,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        min_facet_angle=25,
        max_radius_surface_delaunay_ball=0.15,
        max_circumradius_edge_ratio=2.0,
        verbose=False,
    )

    tol = 0.02
    assert abs(max(mesh.points[:, 0]) - (radius + displacement)) < tol
    assert abs(min(mesh.points[:, 0]) - 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - radius) < tol
    assert abs(min(mesh.points[:, 1]) + radius) < tol
    assert abs(max(mesh.points[:, 2]) - radius) < tol
    assert abs(min(mesh.points[:, 2]) + radius) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    h = radius - displacement
    ref_vol = 4.0 / 3.0 * np.pi * radius**3 - 2 * h * np.pi / 6.0 * (
        3 * a**2 + h**2
    )

    assert abs(vol - ref_vol) < 0.05


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

    mesh = pygalmesh.generate_mesh(
        u,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=0.1,
        verbose=False,
    )

    # filter the vertices that belong to cells
    verts = mesh.points[np.unique(mesh.get_cells_type("tetra"))]

    tol = 1.0e-2
    assert abs(max(verts[:, 0]) - 2.0) < tol
    assert abs(min(verts[:, 0]) - 1.0) < tol
    assert abs(max(verts[:, 1]) - 2.0) < tol
    assert abs(min(verts[:, 1]) - 1.0) < tol
    assert abs(max(verts[:, 2]) - 0.5) < 0.05
    assert abs(min(verts[:, 2]) + 0.5) < 0.05

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 1.0) < 0.05


def test_cuboids_union():
    c0 = pygalmesh.Cuboid([0, 0, -0.5], [3, 3, 0.5])
    c1 = pygalmesh.Cuboid([1, 1, -2], [2, 2, 2])
    u = pygalmesh.Union([c0, c1])

    mesh = pygalmesh.generate_mesh(
        u,
        max_cell_circumradius=0.2,
        max_edge_size_at_feature_edges=0.2,
        verbose=False,
    )

    # filter the vertices that belong to cells
    verts = mesh.points[np.unique(mesh.get_cells_type("tetra"))]

    tol = 1.0e-2
    assert abs(max(verts[:, 0]) - 3.0) < tol
    assert abs(min(verts[:, 0]) - 0.0) < tol
    assert abs(max(verts[:, 1]) - 3.0) < tol
    assert abs(min(verts[:, 1]) - 0.0) < tol
    assert abs(max(verts[:, 2]) - 2.0) < tol
    assert abs(min(verts[:, 2]) + 2.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 12.0) < 0.1


def test_cuboid():
    s0 = pygalmesh.Cuboid([0, 0, 0], [1, 2, 3])
    mesh = pygalmesh.generate_mesh(
        s0, max_edge_size_at_feature_edges=0.1, verbose=False
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 1.0) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 6.0) < tol


def test_cone():
    base_radius = 1.0
    height = 2.0
    max_edge_size_at_feature_edges = 0.1
    s0 = pygalmesh.Cone(base_radius, height, max_edge_size_at_feature_edges)
    mesh = pygalmesh.generate_mesh(
        s0,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        verbose=False,
    )

    tol = 2.0e-1
    assert abs(max(mesh.points[:, 0]) - base_radius) < tol
    assert abs(min(mesh.points[:, 0]) + base_radius) < tol
    assert abs(max(mesh.points[:, 1]) - base_radius) < tol
    assert abs(min(mesh.points[:, 1]) + base_radius) < tol
    assert abs(max(mesh.points[:, 2]) - height) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref_vol = np.pi * base_radius * base_radius / 3.0 * height
    assert abs(vol - ref_vol) < tol


def test_cylinder():
    radius = 1.0
    z0 = 0.0
    z1 = 1.0
    edge_length = 0.1
    s0 = pygalmesh.Cylinder(z0, z1, radius, edge_length)
    mesh = pygalmesh.generate_mesh(
        s0,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=edge_length,
        verbose=False,
    )

    tol = 1.0e-1
    assert abs(max(mesh.points[:, 0]) - radius) < tol
    assert abs(min(mesh.points[:, 0]) + radius) < tol
    assert abs(max(mesh.points[:, 1]) - radius) < tol
    assert abs(min(mesh.points[:, 1]) + radius) < tol
    assert abs(max(mesh.points[:, 2]) - z1) < tol
    assert abs(min(mesh.points[:, 2]) + z0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref_vol = np.pi * radius * radius * (z1 - z0)
    assert abs(vol - ref_vol) < tol


def test_tetrahedron():
    s0 = pygalmesh.Tetrahedron(
        [0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]
    )
    mesh = pygalmesh.generate_mesh(
        s0,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=0.1,
        verbose=False,
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 1.0) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 1.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 1.0 / 6.0) < tol


def test_torus():
    major_radius = 1.0
    minor_radius = 0.5
    s0 = pygalmesh.Torus(major_radius, minor_radius)
    mesh = pygalmesh.generate_mesh(s0, max_cell_circumradius=0.1, verbose=False)

    tol = 1.0e-2
    radii_sum = major_radius + minor_radius
    assert abs(max(mesh.points[:, 0]) - radii_sum) < tol
    assert abs(min(mesh.points[:, 0]) + radii_sum) < tol
    assert abs(max(mesh.points[:, 1]) - radii_sum) < tol
    assert abs(min(mesh.points[:, 1]) + radii_sum) < tol
    assert abs(max(mesh.points[:, 2]) - minor_radius) < tol
    assert abs(min(mesh.points[:, 2]) + minor_radius) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref_vol = (np.pi * minor_radius * minor_radius) * (2 * np.pi * major_radius)
    assert abs(vol - ref_vol) < 1.0e-1


def test_custom_function():
    class Hyperboloid(pygalmesh.DomainBase):
        def __init__(self, max_edge_size_at_feature_edges):
            super().__init__()
            self.z0 = -1.0
            self.z1 = 1.0
            self.waist_radius = 0.5
            self.max_edge_size_at_feature_edges = max_edge_size_at_feature_edges

        def eval(self, x):
            if self.z0 < x[2] and x[2] < self.z1:
                return x[0] ** 2 + x[1] ** 2 - (x[2] ** 2 + self.waist_radius) ** 2
            return 1.0

        def get_bounding_sphere_squared_radius(self):
            z_max = max(abs(self.z0), abs(self.z1))
            r_max = z_max**2 + self.waist_radius
            return r_max * r_max + z_max * z_max

        def get_features(self):
            radius0 = self.z0**2 + self.waist_radius
            n0 = int(2 * np.pi * radius0 / self.max_edge_size_at_feature_edges)
            circ0 = [
                [
                    radius0 * np.cos((2 * np.pi * k) / n0),
                    radius0 * np.sin((2 * np.pi * k) / n0),
                    self.z0,
                ]
                for k in range(n0)
            ]
            circ0.append(circ0[0])

            radius1 = self.z1**2 + self.waist_radius
            n1 = int(2 * np.pi * radius1 / self.max_edge_size_at_feature_edges)
            circ1 = [
                [
                    radius1 * np.cos((2 * np.pi * k) / n1),
                    radius1 * np.sin((2 * np.pi * k) / n1),
                    self.z1,
                ]
                for k in range(n1)
            ]
            circ1.append(circ1[0])
            return [circ0, circ1]

    max_edge_size_at_feature_edges = 0.12
    d = Hyperboloid(max_edge_size_at_feature_edges)

    mesh = pygalmesh.generate_mesh(
        d,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        verbose=False,
    )

    # TODO check the reference values
    tol = 1.0e-1
    assert abs(max(mesh.points[:, 0]) - 1.4) < tol
    assert abs(min(mesh.points[:, 0]) + 1.4) < tol
    assert abs(max(mesh.points[:, 1]) - 1.4) < tol
    assert abs(min(mesh.points[:, 1]) + 1.4) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 1.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 2 * np.pi * 47.0 / 60.0) < 0.16


def test_scaling():
    alpha = 1.3
    s = pygalmesh.Scale(pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), alpha)
    mesh = pygalmesh.generate_mesh(
        s,
        max_cell_circumradius=0.2,
        max_edge_size_at_feature_edges=0.1,
        verbose=False,
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - 1 * alpha) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2 * alpha) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3 * alpha) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 6.0 * alpha**3) < tol


def test_stretch():
    alpha = 2.0
    s = pygalmesh.Stretch(pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), [alpha, 0.0, 0.0])
    mesh = pygalmesh.generate_mesh(
        s,
        max_cell_circumradius=0.2,
        max_edge_size_at_feature_edges=0.2,
        verbose=False,
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - alpha) < tol
    assert abs(min(mesh.points[:, 0]) + 0.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 12.0) < tol


def test_rotation():
    s0 = pygalmesh.Rotate(
        pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), [1.0, 0.0, 0.0], np.pi / 12.0
    )
    mesh = pygalmesh.generate_mesh(
        s0,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=0.1,
        verbose=False,
    )

    tol = 1.0e-2
    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 6.0) < tol


def test_translation():
    s0 = pygalmesh.Translate(pygalmesh.Cuboid([0, 0, 0], [1, 2, 3]), [1.0, 0.0, 0.0])
    mesh = pygalmesh.generate_mesh(
        s0,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=0.1,
        verbose=False,
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - 2.0) < tol
    assert abs(min(mesh.points[:, 0]) - 1.0) < tol
    assert abs(max(mesh.points[:, 1]) - 2.0) < tol
    assert abs(min(mesh.points[:, 1]) + 0.0) < tol
    assert abs(max(mesh.points[:, 2]) - 3.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol
    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 6.0) < tol


def test_extrude():
    p = pygalmesh.Polygon2D([[-0.5, -0.3], [0.5, -0.3], [0.0, 0.5]])
    domain = pygalmesh.Extrude(p, [0.0, 0.3, 1.0])
    mesh = pygalmesh.generate_mesh(
        domain,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=0.1,
        verbose=False,
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 0.5) < tol
    assert abs(min(mesh.points[:, 0]) + 0.5) < tol
    assert abs(max(mesh.points[:, 1]) - 0.8) < tol
    assert abs(min(mesh.points[:, 1]) + 0.3) < tol
    # Relax tolerance for debian, see <https://github.com/nschloe/pygalmesh/pull/47>
    assert abs(max(mesh.points[:, 2]) - 1.0) < 1.1e-3
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 0.4) < tol


def test_extrude_rotate():
    p = pygalmesh.Polygon2D([[-0.5, -0.3], [0.5, -0.3], [0.0, 0.5]])
    max_edge_size_at_feature_edges = 0.1
    domain = pygalmesh.Extrude(
        p,
        [0.0, 0.0, 1.0],
        0.5 * 3.14159265359,
        max_edge_size_at_feature_edges,
    )
    mesh = pygalmesh.generate_mesh(
        domain,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        verbose=False,
    )

    tol = 1.0e-3
    assert abs(max(mesh.points[:, 0]) - 0.583012701892) < tol
    assert abs(min(mesh.points[:, 0]) + 0.5) < tol
    assert abs(max(mesh.points[:, 1]) - 0.5) < tol
    assert abs(min(mesh.points[:, 1]) + 0.583012701892) < tol
    assert abs(max(mesh.points[:, 2]) - 1.0) < tol
    assert abs(min(mesh.points[:, 2]) + 0.0) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 0.4) < 0.05


def test_ring_extrude():
    p = pygalmesh.Polygon2D([[0.5, -0.3], [1.5, -0.3], [1.0, 0.5]])
    max_edge_size_at_feature_edges = 0.1
    domain = pygalmesh.RingExtrude(p, max_edge_size_at_feature_edges)
    mesh = pygalmesh.generate_mesh(
        domain,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        verbose=False,
    )

    tol = 1.0e-2
    assert abs(max(mesh.points[:, 0]) - 1.5) < tol
    assert abs(min(mesh.points[:, 0]) + 1.5) < tol
    assert abs(max(mesh.points[:, 1]) - 1.5) < tol
    assert abs(min(mesh.points[:, 1]) + 1.5) < tol
    assert abs(max(mesh.points[:, 2]) - 0.5) < tol
    assert abs(min(mesh.points[:, 2]) + 0.3) < tol

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 2 * np.pi * 0.4) < 0.05


def test_heart():
    class Heart(pygalmesh.DomainBase):
        def __init__(self, max_edge_size_at_feature_edges):
            super().__init__()

        def eval(self, x):
            return (
                (x[0] ** 2 + 9.0 / 4.0 * x[1] ** 2 + x[2] ** 2 - 1) ** 3
                - x[0] ** 2 * x[2] ** 3
                - 9.0 / 80.0 * x[1] ** 2 * x[2] ** 3
            )

        def get_bounding_sphere_squared_radius(self):
            return 10.0

    max_edge_size_at_feature_edges = 0.1
    d = Heart(max_edge_size_at_feature_edges)

    mesh = pygalmesh.generate_mesh(
        d,
        max_cell_circumradius=0.1,
        max_edge_size_at_feature_edges=max_edge_size_at_feature_edges,
        # odt=True,
        # lloyd=True,
        # verbose=True
    )

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    ref = 3.3180194961823872
    assert abs(vol - ref) < 1.0e-3 * ref


def test_halfspace():
    c = pygalmesh.Cuboid([0, 0, 0], [1, 1, 1])
    s = pygalmesh.HalfSpace([1.0, 2.0, 3.0], 1.0, 2.0)
    u = pygalmesh.Intersection([c, s])

    mesh = pygalmesh.generate_mesh(
        u,
        max_cell_circumradius=0.2,
        max_edge_size_at_feature_edges=0.2,
        verbose=False,
    )

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 1 / 750) < 1.0e-3


def test_ball_with_sizing_field():
    mesh = pygalmesh.generate_mesh(
        pygalmesh.Ball([0.0, 0.0, 0.0], 1.0),
        min_facet_angle=30.0,
        max_radius_surface_delaunay_ball=0.1,
        max_facet_distance=0.025,
        max_circumradius_edge_ratio=2.0,
        max_cell_circumradius=lambda x: abs(np.sqrt(np.dot(x, x)) - 0.5) / 5 + 0.025,
        verbose=False,
    )

    assert abs(max(mesh.points[:, 0]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 0]) + 1.0) < 0.02
    assert abs(max(mesh.points[:, 1]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 1]) + 1.0) < 0.02
    assert abs(max(mesh.points[:, 2]) - 1.0) < 0.02
    assert abs(min(mesh.points[:, 2]) + 1.0) < 0.02

    vol = sum(helpers.compute_volumes(mesh.points, mesh.get_cells_type("tetra")))
    assert abs(vol - 4.0 / 3.0 * np.pi) < 0.15


if __name__ == "__main__":
    test_ball()
    # test_ball_with_sizing_field()
