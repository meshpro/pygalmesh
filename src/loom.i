%module loom

%{
#define SWIG_FILE_WITH_INIT
#include "loom.hpp"
%}


%include <std_shared_ptr.i>

%include <std_vector.i>
namespace std {
%template(Line) vector <double>;
}

%shared_ptr(DomainBase);
%shared_ptr(Ball);
%shared_ptr(Cuboid);
%shared_ptr(Ellipsoid);

%include "loom.hpp"
