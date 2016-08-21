%module loom

%{
#define SWIG_FILE_WITH_INIT
#include "loom.hpp"
%}

%include <std_shared_ptr.i>

%shared_ptr(DomainBase);
%shared_ptr(Sphere);

%include "loom.hpp"
