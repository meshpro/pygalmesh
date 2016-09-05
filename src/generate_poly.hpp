#ifndef GENERATE_POLY_HPP
#define GENERATE_POLY_HPP

#include <string>
#include <vector>

namespace loom {

void
generate_poly(
    const std::string & infile,
    const std::string & outfile,
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

} // namespace loom

#endif // GENERATE_POLY_HPP
