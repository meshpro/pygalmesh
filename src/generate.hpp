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
    const std::vector<std::vector<std::array<double, 3>>> & extra_feature_edges = {},
    const double bounding_sphere_radius = 0.0,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    //
    const double max_edge_size_at_feature_edges_value = 0.0,  // std::numeric_limits<double>::max(),
    const std::shared_ptr<pygalmesh::SizingFieldBase> & max_edge_size_at_feature_edges_field = nullptr,
    //
    const double min_facet_angle = 0.0,
    //
    const double max_radius_surface_delaunay_ball_value = 0.0,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & max_radius_surface_delaunay_ball_field = nullptr,
    //
    const double max_facet_distance_value = 0.0,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & max_facet_distance_field = nullptr,
    //
    const double max_circumradius_edge_ratio = 0.0,
    //
    const double max_cell_circumradius_value = 0.0,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & max_cell_circumradius_field = nullptr,
    //
    const double exude_time_limit = 0.0,
    const double exude_sliver_bound = 0.0,
    //
    const bool verbose = true,
    const int seed = 0
    );

} // namespace pygalmesh

#endif // GENERATE_HPP
