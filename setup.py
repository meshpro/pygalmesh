import os

from setuptools import Extension, setup


class get_pybind_include:
    """Helper class to determine the pybind11 include path. The purpose of this class is
    to postpone importing pybind11 until it is actually installed, so that the
    ``get_include()`` method can be invoked.
    """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11

        return pybind11.get_include(self.user)


ext_modules = [
    Extension(
        "_pygalmesh",
        [
            "src/generate.cpp",
            "src/generate_from_inr.cpp",
            "src/generate_from_off.cpp",
            "src/generate_periodic.cpp",
            "src/generate_surface_mesh.cpp",
            "src/remesh_surface.cpp",
            "src/version.cpp",
            "src/pybind11.cpp",
        ],
        language="c++",
        include_dirs=[
            os.environ.get("EIGEN_INCLUDE_DIR", "/usr/include/eigen3/"),
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
        ],
        extra_compile_args=["-std=c++14"],
        # no CGAL libraries necessary from CGAL 5.0 onwards
        libraries=["gmp", "mpfr"],
    )
]

if __name__ == "__main__":
    setup(
        # cmdclass={'build_ext': BuildExt},
        ext_modules=ext_modules,
    )
