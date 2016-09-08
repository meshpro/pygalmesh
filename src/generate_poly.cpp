#include "generate_poly.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>

// IO
#include <CGAL/IO/Polyhedron_iostream.h>

namespace meshmaker {

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;

typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

void
generate_poly(
    const std::string & infile,
    const std::string & outfile,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude,
    const double edge_size,
    const double facet_angle,
    const double facet_size,
    const double facet_distance,
    const double cell_radius_edge_ratio,
    const double cell_size,
    const bool verbose
    )
{
  std::cout << "a" << std::endl;
  // Create input polyhedron
  Polyhedron polyhedron;
  std::cout << "b" << std::endl;
  std::ifstream input(infile);
  std::cout << "c" << std::endl;
  input >> polyhedron;
  std::cout << "d" << std::endl;
  if (!input.good()) {
    std::cout << "d1" << std::endl;
    std::stringstream msg;
    msg << "Cannot read input file \"" << infile << "\"" << std::endl;
    std::cout << "d2" << std::endl;
    throw std::runtime_error(msg.str());
  }
  input.close();
  std::cout << "e" << std::endl;

  // Create domain
  Mesh_domain cgal_domain(polyhedron);
  std::cout << "f" << std::endl;

  // Mesh criteria
  Mesh_criteria criteria(
      CGAL::parameters::edge_size=edge_size,
      CGAL::parameters::facet_angle=facet_angle,
      CGAL::parameters::facet_size=facet_size,
      CGAL::parameters::facet_distance=facet_distance,
      CGAL::parameters::cell_radius_edge_ratio=cell_radius_edge_ratio,
      CGAL::parameters::cell_size=cell_size
      );
  std::cout << "g" << std::endl;

  // Mesh generation
  if (!verbose) {
    // suppress output
    std::cerr.setstate(std::ios_base::failbit);
  }
  std::cout << "h" << std::endl;
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      cgal_domain,
      criteria,
      lloyd ? CGAL::parameters::lloyd() : CGAL::parameters::no_lloyd(),
      odt ? CGAL::parameters::odt() : CGAL::parameters::no_odt(),
      perturb ? CGAL::parameters::perturb() : CGAL::parameters::no_perturb(),
      exude ? CGAL::parameters::exude() : CGAL::parameters::no_exude()
      );
  std::cout << "i" << std::endl;
  if (!verbose) {
    std::cerr.clear();
  }

  // Output
  std::cout << "y" << std::endl;
  std::ofstream medit_file(outfile);
  c3t3.output_to_medit(medit_file);
  medit_file.close();
  std::cout << "z" << std::endl;

  return;
}

} // namespace meshmaker
