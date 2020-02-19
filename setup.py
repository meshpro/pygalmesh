import codecs
import os

from setuptools import Extension, find_packages, setup

# https://packaging.python.org/single_source_version/
base_dir = os.path.abspath(os.path.dirname(__file__))
about = {}
with open(os.path.join(base_dir, "pygalmesh", "__about__.py"), "rb") as handle:
    exec(handle.read(), about)


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


def read(fname):
    return codecs.open(os.path.join(base_dir, fname), encoding="utf-8").read()


ext_modules = [
    Extension(
        "_pygalmesh",
        [
            "src/generate.cpp",
            "src/generate_from_inr.cpp",
            "src/generate_from_off.cpp",
            "src/generate_periodic.cpp",
            "src/generate_surface_mesh.cpp",
            "src/version.cpp",
            "src/pybind11.cpp",
        ],
        language="C++",
        include_dirs=[
            os.environ.get("EIGEN_INCLUDE_DIR", "/usr/include/eigen3/"),
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
        ],
        # no CGAL libraries necessary from CGAL 5.0 onwards
        libraries=["stdc++", "gmp", "mpfr"],
    )
]

setup(
    name="pygalmesh",
    packages=find_packages(),
    # cmdclass={'build_ext': BuildExt},
    ext_modules=ext_modules,
    version=about["__version__"],
    url=about["__url__"],
    author=about["__author__"],
    author_email=about["__author_email__"],
    setup_requires=["pybind11 >= 2.2"],
    install_requires=["meshio >= 4.0.0, < 5.0.0", "numpy", "pybind11 >= 2.2"],
    python_requires=">=3",
    description="Python frontend to CGAL's 3D mesh generation capabilities",
    long_description=read("README.md"),
    long_description_content_type="text/markdown",
    license=about["__license__"],
    classifiers=[
        about["__status__"],
        about["__license__"],
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Mathematics",
        "Topic :: Scientific/Engineering :: Physics",
        "Topic :: Scientific/Engineering :: Visualization",
    ],
    entry_points={
        "console_scripts": [
            "pygalmesh-volume-from-surface = pygalmesh.cli:volume_from_surface",
            "pygalmesh-from-inr = pygalmesh.cli:inr",
        ]
    },
)
