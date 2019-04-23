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

        def get_bounding_sphere_squared_radius(self):
            return 10.0

    d = Schwarz()

    mesh = pygalmesh.generate_periodic_mesh(
        d,
        cell_size=0.05,
        facet_angle=30,
        facet_size=0.05,
        facet_distance=0.025,
        cell_radius_edge_ratio=2.0,
        number_of_copies_in_output=4,
        # odt=True,
        # lloyd=True,
        # verbose=True
    )

    import meshio
    meshio.write("out.vtk", mesh)
    return


if __name__ == "__main__":
    test_schwarz()
