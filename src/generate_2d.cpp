#define CGAL_MESH_3_VERBOSE 1

#include "generate_2d.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <iostream>

namespace pygalmesh {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;

void
generate_2d(
  const std::vector<std::array<double, 2>> & points,
  const std::vector<std::array<int, 2>> & constraints,
  // See
  // https://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html#a58b0186eae407ba76b8f4a3d0aa85a1a
  // for what the bounds mean. Spoiler:
  // B = circumradius / shortest_edge,
  //   relates to the smallest angle via sin(alpha_min) = 1 / (2B)
  // cell_size is "size",
  const double max_circumradius_shortest_edge_ratio,
  const double cell_size
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
  CGAL::refine_Delaunay_mesh_2(
      cdt,
      Criteria(
        0.25 / (max_circumradius_shortest_edge_ratio * max_circumradius_shortest_edge_ratio),
        cell_size
       )
      );
  std::cout << "Number of vertices after refine_Delaunay_mesh_2: "
            << cdt.number_of_vertices() << std::endl;

  // // make it conforming Delaunay
  // CGAL::make_conforming_Delaunay_2(cdt);
  // std::cout << "Number of vertices after make_conforming_Delaunay_2: "
  //           << cdt.number_of_vertices() << std::endl;
  // // then make it conforming Gabriel
  // CGAL::make_conforming_Gabriel_2(cdt);
  // std::cout << "Number of vertices after make_conforming_Gabriel_2: "
  //           << cdt.number_of_vertices() << std::endl;
}


} // namespace pygalmesh
