%module loom

%{
#define SWIG_FILE_WITH_INIT
#include "generate.hpp"
#include "primitives.hpp"
#include "domain.hpp"
%}

%include <std_vector.i>
namespace std {
%template(Line) vector <double>;
}

%include "std_string.i"

%include <std_shared_ptr.i>
%shared_ptr(loom::DomainBase);
%shared_ptr(loom::PrimitiveBase);
%shared_ptr(loom::Ball);
%shared_ptr(loom::Cuboid);
%shared_ptr(loom::Cylinder);
%shared_ptr(loom::Ellipsoid);

%shared_ptr(loom::RotatedPrimitive);
%shared_ptr(loom::Rotate);

%shared_ptr(loom::Difference);
%shared_ptr(loom::Intersection);
%shared_ptr(loom::Union);

%include "generate.hpp"
%include "primitives.hpp"
%include "domain.hpp"
