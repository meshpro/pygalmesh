#ifndef GENERATE_HPP
#define GENERATE_HPP

#include "domain.hpp"

#include <memory>
#include <string>

namespace loom {

void generate_mesh(
    const std::shared_ptr<loom::DomainBase> & domain,
    const double bounding_sphere_radius,
    const std::string & outfile,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    const double boundary_precision = 1.0e-4,
    const double facet_angle = 30,
    const double facet_size = 0.15,
    const double facet_distance = 0.02,
    const double cell_radius_edge_ratio = 2.0,
    const double cell_size = 0.15
    );
    // const std::vector<std::vector<std::vector<double>>> & feature_edges = {},

} // namespace loom

#endif // GENERATE_HPP
