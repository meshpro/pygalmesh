%module(directors="1") meshmaker

%{
#define SWIG_FILE_WITH_INIT
#include "generate.hpp"
#include "generate_poly.hpp"
#include "generate_surface_mesh.hpp"
#include "domain.hpp"
#include "polygon2d.hpp"
#include "primitives.hpp"
%}

%include "std_string.i"

%include <std_shared_ptr.i>
%shared_ptr(meshmaker::DomainBase);

%shared_ptr(meshmaker::Extrude);
%shared_ptr(meshmaker::Polygon2D);
%shared_ptr(meshmaker::ring_extrude);

%shared_ptr(meshmaker::Rotate);
%shared_ptr(meshmaker::Scale);
%shared_ptr(meshmaker::Stretch);
%shared_ptr(meshmaker::Translate);

%shared_ptr(meshmaker::Difference);
%shared_ptr(meshmaker::Intersection);
%shared_ptr(meshmaker::Union);

%shared_ptr(meshmaker::Ball);
%shared_ptr(meshmaker::Cone);
%shared_ptr(meshmaker::Cuboid);
%shared_ptr(meshmaker::Cylinder);
%shared_ptr(meshmaker::Ellipsoid);
%shared_ptr(meshmaker::Tetrahedron);
%shared_ptr(meshmaker::Torus);

// http://stackoverflow.com/a/39273055/353337
%feature("director") meshmaker::DomainBase;

%include <std_array.i>
%include <std_vector.i>
namespace std {
%template(Vector2d) array<double, 2>;
%template(Vector3d) array<double, 3>;
%template(Polyline2d) vector<array<double, 2>>;
%template(Polyline3d) vector<array<double, 3>>;
%template(ListOfPolylines) vector<vector<array<double, 3>>>;
%template(ListOfDomains) vector<shared_ptr<const meshmaker::DomainBase>>;
}

%include "generate.hpp"
%include "generate_poly.hpp"
%include "generate_surface_mesh.hpp"
%include "domain.hpp"
%include "polygon2d.hpp"
%include "primitives.hpp"
