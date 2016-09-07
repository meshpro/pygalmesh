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
                'meshmaker/generate.cpp',
                'meshmaker/generate_poly.cpp',
                'meshmaker/generate_surface_mesh.cpp',
                'meshmaker/meshmaker.i'
            ],
            include_dirs=['/usr/include/eigen3/'],
            swig_opts=['-keyword', '-c++'],
            extra_compile_args=['-std=c++11']
            )
        ],
    version=__version__,
    packages=['meshmaker'],
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
