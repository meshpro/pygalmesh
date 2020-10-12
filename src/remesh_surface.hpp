#ifndef REMESH_SURFACE_HPP
#define REMESH_SURFACE_HPP

#include <string>
#include <vector>

namespace pygalmesh {

void remesh_surface(
    const std::string & infilen,
    const std::string & outfile,
    const double max_edge_size_at_feature_edges = 0.0,  // std::numeric_limits<double>::max(),
    const double min_facet_angle = 0.0,
    const double max_radius_surface_delaunay_ball = 0.0,
    const double max_facet_distance = 0.0,
    const bool verbose = true,
    const int seed = 0
    );

} // namespace pygalmesh

#endif // REMESH_SURFACE_HPP
