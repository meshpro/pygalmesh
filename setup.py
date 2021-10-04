import os

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

# https://github.com/pybind/python_example/
ext_modules = [
    Pybind11Extension(
        "_pygalmesh",
        # Sort input source files to ensure bit-for-bit reproducible builds
        # (https://github.com/pybind/python_example/pull/53)
        sorted(
            [
                "src/generate.cpp",
                "src/generate_2d.cpp",
                "src/generate_from_inr.cpp",
                "src/generate_from_off.cpp",
                "src/generate_periodic.cpp",
                "src/generate_surface_mesh.cpp",
                "src/remesh_surface.cpp",
                "src/pybind11.cpp",
            ]
        ),
        include_dirs=[
            os.environ.get("EIGEN_INCLUDE_DIR", "/usr/include/eigen3/"),
            # macos/brew:
            "/usr/local/include/eigen3",
        ],
        # no CGAL libraries necessary from CGAL 5.0 onwards
        libraries=["gmp", "mpfr"],
    )
]

if __name__ == "__main__":
    setup(
        cmdclass={"build_ext": build_ext},
        ext_modules=ext_modules,
        zip_safe=False,
    )
