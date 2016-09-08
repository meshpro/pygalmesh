FIND_PACKAGE(CGAL REQUIRED)
find_package(Boost COMPONENTS thread REQUIRED)

FIND_PACKAGE(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

FIND_PACKAGE(PythonLibs 2 REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "-keyword")

SET_SOURCE_FILES_PROPERTIES(meshmaker.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(
  meshmaker python meshmaker.i
  generate.cpp
  generate_poly.cpp
  generate_surface_mesh.cpp
  )
SWIG_LINK_LIBRARIES(
  meshmaker
  ${PYTHON_LIBRARIES}
  ${CGAL_LIBRARY}
  ${GMP_LIBRARIES}
  ${MPFR_LIBRARIES}
  ${Boost_LIBRARIES}
  )

execute_process(
  COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
install(TARGETS _meshmaker DESTINATION ${PYTHON_SITE_PACKAGES})
install(
  FILES ${CMAKE_BINARY_DIR}/src/meshmaker.py
  DESTINATION ${PYTHON_SITE_PACKAGES}
  )
