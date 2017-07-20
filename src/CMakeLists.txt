FIND_PACKAGE(CGAL REQUIRED)
include(${CGAL_USE_FILE})

FIND_PACKAGE(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

FILE(GLOB frentos_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# FILE(GLOB frentos_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

FIND_PACKAGE(pybind11 REQUIRED)
# include_directories(${PYBIND11_INCLUDE_DIR})

pybind11_add_module(frentos ${frentos_SRCS})
# ADD_LIBRARY(frentos ${frentos_SRCS})
target_link_libraries(frentos PRIVATE ${CGAL_LIBRARIES})

# execute_process(
#   COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
#   OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
#   OUTPUT_STRIP_TRAILING_WHITESPACE
#   )
# install(TARGETS _frentos DESTINATION ${PYTHON_SITE_PACKAGES})
# install(
#   FILES ${CMAKE_BINARY_DIR}/src/frentos.py
#   DESTINATION ${PYTHON_SITE_PACKAGES}
#   )
