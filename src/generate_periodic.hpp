#ifndef GENERATE_PERIODIC_HPP
#define GENERATE_PERIODIC_HPP

#include "domain.hpp"

#include <memory>
#include <string>
#include <vector>

namespace pygalmesh {

void generate_periodic_mesh(
    const std::shared_ptr<pygalmesh::DomainBase> & domain,
    const std::string & outfile,
    const std::array<double, 6> bounding_cuboid,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    const double max_edge_size_at_feature_edges = 0.0,  // std::numeric_limits<double>::max(),
    const double min_facet_angle = 0.0,
    const double max_radius_surface_delaunay_ball = 0.0,
    const double max_facet_distance = 0.0,
    const double max_circumradius_edge_ratio = 0.0,
    const double max_cell_circumradius = 0.0,
    const int number_of_copies_in_output = 1,
    const bool verbose = true,
    const int seed = 0
    );

} // namespace pygalmesh

#endif // GENERATE_PERIODIC_HPP
