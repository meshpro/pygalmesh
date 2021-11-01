eigen_includes = include_directories('/usr/include/eigen3')
cgal_dep = dependency('CGAL')

pymod = import('python')
py3 = pymod.find_installation('python3')
pybind11_dep = dependency('pybind11', fallback: ['pybind11', 'pybind11_dep'])

py3.extension_module(
  '_pygalmesh',
  'generate.cpp',
  'generate_2d.cpp',
  'generate_from_inr.cpp',
  'generate_from_off.cpp',
  'generate_periodic.cpp',
  'generate_surface_mesh.cpp',
  'pybind11.cpp',
  'remesh_surface.cpp',
  include_directories: eigen_includes,
  dependencies : [cgal_dep, pybind11_dep]
)
