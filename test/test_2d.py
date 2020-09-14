import numpy

import pygalmesh


def test_2d():
    points = numpy.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
    constraints = [[0, 1], [1, 2], [2, 3], [3, 0]]

    points, cells = pygalmesh.generate_2d(points, constraints, cell_size=1.0e-1)

    assert points.shape == (276, 2)
    assert cells.shape == (486, 3)


if __name__ == "__main__":
    test_2d()
