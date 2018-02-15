# -*- coding: utf-8 -*-
#
import os
import sys

import codecs
import setuptools
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


__version__ = '0.2.0'
__author__ = u'Nico Schlömer'
__author_email__ = 'nico.schloemer@gmail.com'
__status__ = 'Development Status :: 3 - Alpha'
__license__ = 'License :: OSI Approved :: MIT License'
__url__ = 'https://github.com/nschloe/pygalmesh'


# https://github.com/pybind/python_example/blob/master/setup.py
class BuildExt(build_ext):
    '''A custom build extension for adding compiler-specific options.'''
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
        }

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append(
                '-DVERSION_INFO=\'%s\'' % self.distribution.get_version()
                )
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append(
                '/DVERSION_INFO=\'%s\'' % self.distribution.get_version()
                )
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)


class get_pybind_include(object):
    '''Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked.
    '''
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    '''Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    '''
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    '''Return the -std=c++[11/14] compiler flag.
    The c++14 is prefered over c++11 (when it is available).
    '''
    if has_flag(compiler, '-std=c++14'):
        return '-std=c++14'
    elif has_flag(compiler, '-std=c++11'):
        return '-std=c++11'
    else:
        raise RuntimeError('Unsupported compiler -- at least C++11 support '
                           'is needed!')


def read(fname):
    # `pip install` doesn't generate README.rst and fails, so intercept with
    # try-execpt.
    try:
        content = codecs.open(
            os.path.join(os.path.dirname(__file__), fname),
            encoding='utf-8'
            ).read()
    except IOError:
        content = ''
    return content


ext_modules = [Extension(
    'pygalmesh',
    [
        'src/generate.cpp',
        'src/generate_from_off.cpp',
        'src/generate_surface_mesh.cpp',
        'src/pybind11.cpp',
        ],
    language='c++',
    include_dirs=[
        '/usr/include/eigen3/',
        # Path to pybind11 headers
        get_pybind_include(),
        get_pybind_include(user=True)
        ],
    libraries=['CGAL', 'gmp', 'mpfr'],
    # extra_compile_args=['-std=c++11']
    )]

setup(
    name='pygalmesh',
    cmdclass={'build_ext': BuildExt},
    ext_modules=ext_modules,
    package_dir={'': 'src'},
    version=__version__,
    url=__url__,
    download_url='https://pypi.python.org/pypi/pygalmesh',
    author=__author__,
    author_email=__author_email__,
    install_requires=[
        'numpy',
        'pybind11'
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
