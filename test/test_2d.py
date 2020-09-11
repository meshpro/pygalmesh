import numpy

import pygalmesh


def test_2d():
    points = numpy.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
    constraints = [[0, 1], [1, 2], [2, 3], [3, 0]]

    pygalmesh.generate_2d(points, constraints, cell_size=1.0e-2)
    exit(1)


if __name__ == "__main__":
    test_2d()
