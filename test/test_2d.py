import numpy

import pygalmesh


def test_2d():
    points = numpy.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
    constraints = [[0, 1], [1, 2], [2, 3], [3, 0]]

    points, cells = pygalmesh.generate_2d(points, constraints, cell_size=1.0e-1)

    assert points.shape == (276, 2)
    assert cells.shape == (486, 3)

    # show mesh
    # import matplotlib.pyplot as plt
    # pts = points[cells]
    # for pt in pts:
    #     plt.plot([pt[0][0], pt[1][0]], [pt[0][1], pt[1][1]], "-k")
    #     plt.plot([pt[1][0], pt[2][0]], [pt[1][1], pt[2][1]], "-k")
    #     plt.plot([pt[2][0], pt[0][0]], [pt[2][1], pt[0][1]], "-k")
    # # for pt in points:
    # #     plt.plot(pt[0], pt[1], "or")
    # plt.gca().set_aspect("equal")
    # plt.show()




if __name__ == "__main__":
    test_2d()
