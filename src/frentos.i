%module(directors="1") frentos

%pythoncode %{
__version__ = '0.1.4'
__license__ = 'MIT License'
__author__ = 'Nico Schlömer'
__email__ = 'nico.schloemer@gmail.com'
%}

%{
#define SWIG_FILE_WITH_INIT
#include "generate.hpp"
#include "generate_from_off.hpp"
#include "generate_surface_mesh.hpp"
#include "domain.hpp"
#include "polygon2d.hpp"
#include "primitives.hpp"
%}

%include "std_string.i"

%include <std_shared_ptr.i>
%shared_ptr(frentos::DomainBase);

%shared_ptr(frentos::Extrude);
%shared_ptr(frentos::Polygon2D);
%shared_ptr(frentos::ring_extrude);

%shared_ptr(frentos::Rotate);
%shared_ptr(frentos::Scale);
%shared_ptr(frentos::Stretch);
%shared_ptr(frentos::Translate);

%shared_ptr(frentos::Difference);
%shared_ptr(frentos::Intersection);
%shared_ptr(frentos::Union);

%shared_ptr(frentos::Ball);
%shared_ptr(frentos::Cone);
%shared_ptr(frentos::Cuboid);
%shared_ptr(frentos::Cylinder);
%shared_ptr(frentos::Ellipsoid);
%shared_ptr(frentos::Tetrahedron);
%shared_ptr(frentos::Torus);

// http://stackoverflow.com/a/39273055/353337
%feature("director") frentos::DomainBase;

%include <std_array.i>
%include <std_vector.i>
namespace std {
%template(Vector2d) array<double, 2>;
%template(Vector3d) array<double, 3>;
%template(Polyline2d) vector<array<double, 2>>;
%template(Polyline3d) vector<array<double, 3>>;
%template(ListOfPolylines) vector<vector<array<double, 3>>>;
%template(ListOfDomains) vector<shared_ptr<const frentos::DomainBase>>;
}

%include "generate.hpp"
%include "generate_from_off.hpp"
%include "generate_surface_mesh.hpp"
%include "domain.hpp"
%include "polygon2d.hpp"
%include "primitives.hpp"
