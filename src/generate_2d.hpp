#ifndef GENERATE_2D_HPP
#define GENERATE_2D_HPP

#include <memory>
#include <vector>

namespace pygalmesh {

std::tuple<std::vector<std::array<double, 2>>, std::vector<std::array<int, 3>>>
generate_2d(
  const std::vector<std::array<double, 2>> & points,
  const std::vector<std::array<int, 2>> & constraints,
  const double max_circumradius_shortest_edge_ratio,
  // https://github.com/CGAL/cgal/issues/5061#issuecomment-705520984
  // the "default" size criterion for a triangle in the 2D mesh generator refers to its
  // edge lengths. In the output mesh, all segments of all triangles must be shorter
  // than the given bound.
  const double max_edge_size,
  const int num_lloyd_steps
);

} // namespace pygalmesh

#endif // GENERATE_2D_HPP
