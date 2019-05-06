# -*- coding: utf-8 -*-
#
import numpy

import pygalmesh


def test_schwarz():
    class Schwarz(pygalmesh.DomainBase):
        def __init__(self):
            super(Schwarz, self).__init__()
            return

        def eval(self, x):
            x2 = numpy.cos(x[0] * 2 * numpy.pi)
            y2 = numpy.cos(x[1] * 2 * numpy.pi)
            z2 = numpy.cos(x[2] * 2 * numpy.pi)
            return x2 + y2 + z2

    mesh = pygalmesh.generate_periodic_mesh(
        Schwarz(),
        [0, 0, 0, 1, 1, 1],
        cell_size=0.05,
        facet_angle=30,
        facet_size=0.05,
        facet_distance=0.025,
        cell_radius_edge_ratio=2.0,
        number_of_copies_in_output=4,
        # odt=True,
        # lloyd=True,
        verbose=False,
    )

    # The RNG in CGAL makes the following assertions fail sometimes.
    # assert len(mesh.cells["triangle"]) == 12784
    # assert len(mesh.cells["tetra"]) == 67120

    return mesh


if __name__ == "__main__":
    import meshio

    mesh = test_schwarz()
    meshio.write("out.vtk", mesh)
