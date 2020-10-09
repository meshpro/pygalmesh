# https://github.com/pybind/pybind11/issues/1004
from _pygalmesh import (
    Ball,
    Cone,
    Cuboid,
    Cylinder,
    Difference,
    DomainBase,
    Ellipsoid,
    Extrude,
    HalfSpace,
    Intersection,
    Polygon2D,
    RingExtrude,
    Rotate,
    Scale,
    Stretch,
    Tetrahedron,
    Torus,
    Translate,
    Union,
)

from . import _cli
from .__about__ import __cgal_version__, __version__
from .main import (
    generate_2d,
    generate_from_array,
    generate_from_inr,
    generate_mesh,
    generate_periodic_mesh,
    generate_surface_mesh,
    generate_volume_mesh_from_surface_mesh,
    remesh_surface,
    save_inr,
)

__all__ = [
    "__version__",
    "__cgal_version__",
    #
    "_cli",
    #
    "DomainBase",
    "Translate",
    "Rotate",
    "Scale",
    "Stretch",
    "Intersection",
    "Union",
    "Difference",
    "Extrude",
    "Ball",
    "Cuboid",
    "Ellipsoid",
    "Tetrahedron",
    "Cone",
    "Cylinder",
    "Torus",
    "HalfSpace",
    "Polygon2D",
    "RingExtrude",
    #
    "generate_mesh",
    "generate_2d",
    "generate_periodic_mesh",
    "generate_surface_mesh",
    "generate_volume_mesh_from_surface_mesh",
    "generate_from_array",
    "generate_from_inr",
    "remesh_surface",
    "save_inr",
]
