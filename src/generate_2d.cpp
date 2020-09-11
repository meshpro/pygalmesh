#define CGAL_MESH_3_VERBOSE 1

#include "generate_2d.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <iostream>

namespace pygalmesh {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;
typedef CDT::Vertex_handle Vertex_handle;

void
generate_2d(
  const std::vector<std::array<double, 2>> & points,
  const std::vector<std::array<int, 2>> & constraints
)
{
  CDT cdt;
  // construct a constrained triangulation
  std::vector<Vertex_handle> vertices(points.size());
  int k = 0;
  for (auto pt: points) {
    vertices[k] = cdt.insert(Point(pt[0], pt[1]));
    k += 1;
  }
  for (auto c: constraints) {
    cdt.insert_constraint(vertices[c[0]], vertices[c[1]]);
  }

  std::cout << "Number of vertices before: "
            << cdt.number_of_vertices() << std::endl;
  // make it conforming Delaunay
  CGAL::make_conforming_Delaunay_2(cdt);
  std::cout << "Number of vertices after make_conforming_Delaunay_2: "
            << cdt.number_of_vertices() << std::endl;
  // then make it conforming Gabriel
  CGAL::make_conforming_Gabriel_2(cdt);
  std::cout << "Number of vertices after make_conforming_Gabriel_2: "
            << cdt.number_of_vertices() << std::endl;
}


} // namespace pygalmesh
