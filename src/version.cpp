#include "version.hpp"

#include <CGAL/version.h>

#include <string>

namespace pygalmesh {

std::string
get_cgal_version()
{
  return CGAL_VERSION_STR;
}

} // namespace pygalmesh
