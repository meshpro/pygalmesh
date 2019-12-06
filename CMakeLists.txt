# CMake is used for debugging in pygalmesh. Like every other Python package, the
# production build system is setuptools.
cmake_minimum_required(VERSION 3.0)

project(pygalmesh CXX)

add_subdirectory(src)
