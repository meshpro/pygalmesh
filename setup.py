# -*- coding: utf-8 -*-
#
from distutils.core import setup, Extension
import os
import codecs

# from meshmaker import __version__, __license__, __author__, __email__
__version__ = '0.1.0'
__license__ = 'MIT'
__author__ = 'Nico Schlömer'
__email__ = 'nico.schloemer@gmail.com'


def read(fname):
    try:
        content = codecs.open(
            os.path.join(os.path.dirname(__file__), fname),
            encoding='utf-8'
            ).read()
    except Exception:
        content = ''
    return content

setup(
    name='meshmaker',
    ext_modules=[
        Extension(
            '_meshmaker',
            [
                'src/generate.cpp',
                'src/generate_poly.cpp',
                'src/generate_surface_mesh.cpp',
                'src/meshmaker.i'
            ],
            include_dirs=['/usr/include/eigen3/'],
            libraries=['CGAL', 'gmp', 'mpfr'],
            swig_opts=['-keyword', '-c++', '-modern'],
            extra_compile_args=['-std=c++11']
            )
        ],
    py_modules=['meshmaker'],
    package_dir={'': 'src'},
    version=__version__,
    url='https://github.com/nschloe/meshmaker',
    download_url='https://pypi.python.org/pypi/meshmaker',
    author=__author__,
    author_email=__email__,
    requires=['numpy'],
    description='3D mesh generation',
    long_description=read('README.rst'),
    license=__license__,
    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Topic :: Scientific/Engineering'
        ]
    )
