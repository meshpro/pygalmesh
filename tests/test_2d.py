import numpy as np
from helpers import compute_triangle_areas

import pygalmesh


def test_rectangle():
    points = np.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
    constraints = [[0, 1], [1, 2], [2, 3], [3, 0]]

    mesh = pygalmesh.generate_2d(
        points, constraints, max_edge_size=1.0e-1, num_lloyd_steps=10
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


def test_disk():
    h = 0.1
    n = int(2 * np.pi / h)
    alpha = np.linspace(0.0, 2 * np.pi, n + 1, endpoint=False)
    points = np.column_stack([np.cos(alpha), np.sin(alpha)])

    constraints = [[k, k + 1] for k in range(n)] + [[n, 0]]
    mesh = pygalmesh.generate_2d(
        points, constraints, max_edge_size=h, num_lloyd_steps=0
    )
    areas = compute_triangle_areas(mesh.points, mesh.get_cells_type("triangle"))
    assert np.all(areas > 1.0e-5)


if __name__ == "__main__":
    test_disk()
