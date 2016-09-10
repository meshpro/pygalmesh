# -*- coding: utf-8 -*-
#
from distutils.core import setup, Extension
from distutils.command.build import build
import os
import codecs

__version__ = '0.1.3'
__license__ = 'MIT License'
__author__ = 'Nico Schlömer'
__email__ = 'nico.schloemer@gmail.com'


class CustomBuild(build):
    # <http://stackoverflow.com/a/21236111/353337>
    sub_commands = [
        ('build_ext', build.has_ext_modules),
        ('build_py', build.has_pure_modules),
        ('build_clib', build.has_c_libraries),
        ('build_scripts', build.has_scripts),
    ]


def read(fname):
    # `pip install` doesn't generate README.rst and fails, so intercept with
    # try-execpt.
    try:
        content = codecs.open(
            os.path.join(os.path.dirname(__file__), fname),
            encoding='utf-8'
            ).read()
    except:
        content = ''
    return content


setup(
    name='frentos',
    cmdclass={'build': CustomBuild},
    py_modules=['frentos'],
    ext_modules=[Extension(
        '_frentos',
        [
            'src/generate.cpp',
            'src/generate_from_off.cpp',
            'src/generate_surface_mesh.cpp',
            'src/frentos.i'
            ],
        include_dirs=['/usr/include/eigen3/'],
        libraries=['CGAL', 'gmp', 'mpfr'],
        swig_opts=['-keyword', '-c++', '-modern'],
        extra_compile_args=['-std=c++11']
        )],
    package_dir={'': 'src'},
    version=__version__,
    url='https://github.com/nschloe/frentos',
    download_url='https://pypi.python.org/pypi/frentos',
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
