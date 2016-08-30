%module loom

%{
#define SWIG_FILE_WITH_INIT
#include "generate.hpp"
#include "domain.hpp"
#include "primitives.hpp"
%}

%include "std_string.i"

%include <std_shared_ptr.i>
%shared_ptr(loom::DomainBase);

%shared_ptr(loom::Rotate);

%shared_ptr(loom::Difference);
%shared_ptr(loom::Intersection);
%shared_ptr(loom::Union);

%shared_ptr(loom::Ball);
%shared_ptr(loom::Cone);
%shared_ptr(loom::Cuboid);
%shared_ptr(loom::Cylinder);
%shared_ptr(loom::Ellipsoid);

%include <std_vector.i>
namespace std {
%template(Line) vector <double>;
%template(ListOfListsOfLists) vector<vector<vector<double>>>;
%template(ListOfDomains) vector<shared_ptr<const loom::DomainBase>>;
}

%include "generate.hpp"
%include "domain.hpp"
%include "primitives.hpp"
