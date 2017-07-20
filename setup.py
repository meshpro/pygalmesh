# -*- coding: utf-8 -*-
#
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os
import codecs

__version__ = '0.1.5'
__author__ = u'Nico Schlömer'
__author_email__ = 'nico.schloemer@gmail.com'
__status__ = 'Development Status :: 3 - Alpha'
__license__ = 'License :: OSI Approved :: MIT License'
__url__ = 'https://github.com/nschloe/frentos'


class BuildExt(build_ext):
    # <http://stackoverflow.com/a/21236111/353337>
    sub_commands = [
        ('build_ext', build_ext.has_ext_modules),
        ('build_py', build_ext.has_pure_modules),
        ('build_clib', build_ext.has_c_libraries),
        ('build_scripts', build_ext.has_scripts),
        ]


# class get_pybind_include(object):
#     '''Helper class to determine the pybind11 include path
#     The purpose of this class is to postpone importing pybind11
#     until it is actually installed, so that the ``get_include()``
#     method can be invoked.
#     '''
#     def __init__(self, user=False):
#         self.user = user
#
#     def __str__(self):
#         import pybind11
#         return pybind11.get_include(self.user)


def read(fname):
    # `pip install` doesn't generate README.rst and fails, so intercept with
    # try-execpt.
    try:
        content = codecs.open(
            os.path.join(os.path.dirname(__file__), fname),
            encoding='utf-8'
            ).read()
    except Exception:
        content = ''
    return content


setup(
    name='frentos',
    cmdclass={'build_ext': BuildExt},
    py_modules=['frentos'],
    ext_modules=[Extension(
        '_frentos',
        [
            'src/generate.cpp',
            'src/generate_from_off.cpp',
            'src/generate_surface_mesh.cpp',
            'src/frentos.i'
        ],
        language='c++',
        include_dirs=[
            '/usr/include/eigen3/',
            # Path to pybind11 headers
            # get_pybind_include(),
            # get_pybind_include(user=True)
            ],
        libraries=['CGAL', 'gmp', 'mpfr'],
        swig_opts=['-keyword', '-c++', '-modern'],
        extra_compile_args=['-std=c++11']
        )],
    package_dir={'': 'src'},
    version=__version__,
    url=__url__,
    download_url='https://pypi.python.org/pypi/frentos',
    author=__author__,
    author_email=__author_email__,
    install_requires=[
        'numpy'
        ],
    description='3D mesh generation',
    long_description=read('README.rst'),
    license=__license__,
    classifiers=[
        __status__,
        __license__,
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Topic :: Scientific/Engineering'
        ]
    )
