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
    const double edge_size = 0.0,  // std::numeric_limits<double>::max(),
    const double facet_angle = 0.0,
    const double facet_size = 0.0,
    const double facet_distance = 0.0,
    const double cell_radius_edge_ratio = 0.0,
    const double cell_size = 0.0,
    const int number_of_copies_in_output = 1,
    const bool verbose = true
    );

} // namespace pygalmesh

#endif // GENERATE_PERIODIC_HPP
