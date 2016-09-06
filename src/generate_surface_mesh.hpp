#ifndef GENERATE_SURFACE_MESH_HPP
#define GENERATE_SURFACE_MESH_HPP

#include "domain.hpp"

#include <memory>
#include <string>

namespace loom {

void generate_surface_mesh(
    const std::shared_ptr<loom::DomainBase> & domain,
    const std::string & outfile,
    const std::vector<std::vector<std::vector<double>>> & feature_edges = {},
    const double bounding_sphere_radius = 0.0,
    const double boundary_precision = 1.0e-4,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    const double edge_size = 0.0,
    const double facet_angle = 0.0,
    const double facet_size = 0.0,
    const double facet_distance = 0.0,
    const double cell_radius_edge_ratio = 0.0,
    const double cell_size = 0.0,
    const bool verbose = true
    );

} // namespace loom

#endif // GENERATE_SURFACE_MESH_HPP
