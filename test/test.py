# -*- coding: utf-8 -*-
#
import loom


def test_ball():
    s = loom.Ball([0, 0, 0], 1.0)
    bounding_sphere_radius = 5.0
    loom.generate_mesh(s, bounding_sphere_radius)
    return


def test_balls():
    s0 = loom.Ball([0.5, 0, 0], 1.0)
    s1 = loom.Ball([-0.5, 0, 0], 1.0)
    u = loom.Union(s0, s1)
    loom.generate_mesh(u, 5.0)
    return


if __name__ == '__main__':
    test_balls()
