import numpy

import pygalmesh


def test_2d():
    points = numpy.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
    constraints = [[0, 1], [1, 2], [2, 3], [3, 0]]

    mesh = pygalmesh.generate_2d(
        points, constraints, edge_size=1.0e-1, num_lloyd_steps=10
    )

    assert mesh.points.shape == (276, 2)
    assert mesh.get_cells_type("triangle").shape == (486, 3)

    # # show mesh
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

    # mesh.points *= 100
    # mesh.write("rect.svg")


if __name__ == "__main__":
    test_2d()
