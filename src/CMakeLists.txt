FIND_PACKAGE(CGAL REQUIRED)
include(${CGAL_USE_FILE})

FIND_PACKAGE(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

FILE(GLOB pygalmesh_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# FILE(GLOB pygalmesh_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

FIND_PACKAGE(pybind11 REQUIRED)
# include_directories(${PYBIND11_INCLUDE_DIR})

pybind11_add_module(pygalmesh ${pygalmesh_SRCS})
# ADD_LIBRARY(pygalmesh ${pygalmesh_SRCS})
target_link_libraries(pygalmesh PRIVATE ${CGAL_LIBRARIES})

# execute_process(
#   COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
#   OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
#   OUTPUT_STRIP_TRAILING_WHITESPACE
#   )
# install(TARGETS _pygalmesh DESTINATION ${PYTHON_SITE_PACKAGES})
# install(
#   FILES ${CMAKE_BINARY_DIR}/src/pygalmesh.py
#   DESTINATION ${PYTHON_SITE_PACKAGES}
#   )
