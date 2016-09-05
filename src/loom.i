%module(directors="1") loom

%{
#define SWIG_FILE_WITH_INIT
#include "generate.hpp"
#include "generate_poly.hpp"
#include "domain.hpp"
#include "polygon2d.hpp"
#include "primitives.hpp"
%}

%include "std_string.i"

%include <std_shared_ptr.i>
%shared_ptr(loom::DomainBase);

%shared_ptr(loom::Extrude);
%shared_ptr(loom::Polygon2D);
%shared_ptr(loom::ring_extrude);

%shared_ptr(loom::Rotate);
%shared_ptr(loom::Scale);
%shared_ptr(loom::Stretch);
%shared_ptr(loom::Translate);

%shared_ptr(loom::Difference);
%shared_ptr(loom::Intersection);
%shared_ptr(loom::Union);

%shared_ptr(loom::Ball);
%shared_ptr(loom::Cone);
%shared_ptr(loom::Cuboid);
%shared_ptr(loom::Cylinder);
%shared_ptr(loom::Ellipsoid);
%shared_ptr(loom::Tetrahedron);
%shared_ptr(loom::Torus);

// http://stackoverflow.com/a/39273055/353337
%feature("director") loom::DomainBase;

%include <std_vector.i>
namespace std {
%template(Vector3d) vector<double>;
%template(Polyline) vector<vector<double>>;
%template(ListOfPolylines) vector<vector<vector<double>>>;
%template(ListOfDomains) vector<shared_ptr<const loom::DomainBase>>;
}

%include "generate.hpp"
%include "generate_poly.hpp"
%include "domain.hpp"
%include "polygon2d.hpp"
%include "primitives.hpp"
