#define CGAL_MESH_3_VERBOSE 1

#include "remesh_surface.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>
#include <CGAL/make_mesh_3.h>

namespace pygalmesh {
// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedral_mesh_domain_with_features_3<K> Mesh_domain;
// Polyhedron type
typedef CGAL::Mesh_polyhedron_3<K>::type Polyhedron;
// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<
  Tr,Mesh_domain::Corner_index,Mesh_domain::Curve_index> C3t3;
// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// <https://doc.cgal.org/latest/Mesh_3/#title24>
void
remesh_surface(
    const std::string & infile,
    const std::string & outfile,
    const double edge_size,
    const double facet_angle,
    const double facet_size,
    const double facet_distance,
    const bool verbose,
    const int seed
    )
{
  CGAL::get_default_random() = CGAL::Random(seed);

  // Load a polyhedron
  Polyhedron poly;
  std::ifstream input(infile.c_str());
  input >> poly;
  if (!CGAL::is_triangle_mesh(poly)){
    throw "Input geometry is not triangulated.";
  }
  // Create a vector with only one element: the pointer to the polyhedron.
  std::vector<Polyhedron*> poly_ptrs_vector(1, &poly);
  // Create a polyhedral domain with only one polyhedron and no "bounding polyhedron"
  // so the volumetric part of the domain will be empty.
  Mesh_domain domain(poly_ptrs_vector.begin(), poly_ptrs_vector.end());

  // Get sharp features
  domain.detect_features(); //includes detection of borders
  // Mesh criteria
  Mesh_criteria criteria(
      CGAL::parameters::edge_size=edge_size,
      CGAL::parameters::facet_angle=facet_angle,
      CGAL::parameters::facet_size=facet_size,
      CGAL::parameters::facet_distance=facet_distance
  );

  // Mesh generation
  if (!verbose) {
    // suppress output
    std::cerr.setstate(std::ios_base::failbit);
  }
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      domain,
      criteria,
      CGAL::parameters::no_perturb(),
      CGAL::parameters::no_exude()
  );
  if (!verbose) {
    std::cerr.clear();
  }
  // Output the facets of the c3t3 to an OFF file. The facets will not be
  // oriented.
  std::ofstream off_file(outfile.c_str());
  c3t3.output_boundary_to_off(off_file);
  if (off_file.fail()) {
    throw "Failed to write OFF.";
  }
  return;
}

} // namespace pygalmesh
