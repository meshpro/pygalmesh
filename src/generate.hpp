#ifndef GENERATE_HPP
#define GENERATE_HPP

#include "domain.hpp"

#include <memory>
#include <string>
#include <vector>

namespace pygalmesh {

void generate_mesh(
    const std::shared_ptr<pygalmesh::DomainBase> & domain,
    const std::string & outfile,
    const std::vector<std::vector<std::array<double, 3>>> & feature_edges = {},
    const double bounding_sphere_radius = 0.0,
    const double boundary_precision = 1.0e-4,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    const double edge_size = 0.0,  // std::numeric_limits<double>::max(),
    const double facet_angle = 0.0,
    const double facet_size = 0.0,
    const double facet_distance = 0.0,
    const double cell_radius_edge_ratio = 0.0,
    const double cell_size = 0.0,
    const bool verbose = true
    );

} // namespace pygalmesh

#endif // GENERATE_HPP
