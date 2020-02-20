#ifndef REMESH_SURFACE_HPP
#define REMESH_SURFACE_HPP

#include <string>
#include <vector>

namespace pygalmesh {

void remesh_surface(
    const std::string & infilen,
    const std::string & outfile,
    const double edge_size = 0.0,  // std::numeric_limits<double>::max(),
    const double facet_angle = 0.0,
    const double facet_size = 0.0,
    const double facet_distance = 0.0,
    const bool verbose = true
    );

} // namespace pygalmesh

#endif // REMESH_SURFACE_HPP
