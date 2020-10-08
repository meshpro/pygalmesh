#ifndef GENERATE_HPP
#define GENERATE_HPP

#include "domain.hpp"
#include "sizing_field.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace pygalmesh {

void generate_mesh(
    const std::shared_ptr<pygalmesh::DomainBase> & domain,
    const std::string & outfile,
    const std::vector<std::vector<std::array<double, 3>>> & feature_edges = {},
    const double bounding_sphere_radius = 0.0,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    //
    const double edge_size_value = 0.0,  // std::numeric_limits<double>::max(),
    const std::shared_ptr<pygalmesh::SizingFieldBase> & edge_size_field = nullptr,
    //
    const double facet_angle = 0.0,
    //
    const double facet_size_value = 0.0,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & facet_size_field = nullptr,
    //
    const double facet_distance_value = 0.0,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & facet_distance_field = nullptr,
    //
    const double cell_radius_edge_ratio = 0.0,
    //
    const double cell_size_value = 0.0,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & cell_size_field = nullptr,
    //
    const bool verbose = true,
    const int seed = 0
    );

} // namespace pygalmesh

#endif // GENERATE_HPP
