#include "generate_from_off.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>

// IO
#include <CGAL/IO/Polyhedron_iostream.h>

// for re-orientation
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>

#include <CGAL/version_macros.h>

#if CGAL_VERSION_MAJOR >= 5 && CGAL_VERSION_MINOR < 3
  #include <CGAL/IO/OFF_reader.h>
#endif

// for sharp features
//#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>

namespace pygalmesh {

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Mesh_domain;
// for sharp features
//typedef CGAL::Polyhedral_mesh_domain_with_features_3<K> Mesh_domain;
//typedef CGAL::Mesh_polyhedron_3<K>::type Polyhedron;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;

typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

void generate_from_off(
    const std::string& infile,
    const std::string& outfile,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude,
    const double max_edge_size_at_feature_edges,
    const double min_facet_angle,
    const double max_radius_surface_delaunay_ball,
    const double max_facet_distance,
    const double max_circumradius_edge_ratio,
    const double max_cell_circumradius,
    const double exude_time_limit,
    const double exude_sliver_bound,
    const bool verbose,
    const bool reorient,
    const int seed
) {
  CGAL::get_default_random() = CGAL::Random(seed);

  std::ifstream input(infile);
  Polyhedron polyhedron;
  // fix the orientation of the faces of the input file
  if (reorient) {
    std::stringstream msg;
    msg << "fixing face orientation for \"" << infile <<"\""<< std::endl;
    std::vector<K::Point_3> points;
    std::vector<std::vector<std::size_t> > polygons;

    if(
      !input ||
#if CGAL_VERSION_MAJOR >= 5 && CGAL_VERSION_MINOR >= 3
      !CGAL::IO::read_OFF(input, points, polygons) ||
#else
      !CGAL::read_OFF(input, points, polygons) ||
#endif
      points.empty()
    )
    {
      std::stringstream msg;
      msg << "Cannot read .off file \"" << infile <<"\""<< std::endl;
      throw std::runtime_error(msg.str());
    }
    // orient the polygons
    CGAL::Polygon_mesh_processing::orient_polygon_soup(points, polygons);
    // create the polyhedron
    CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, polyhedron);

  } else {

    // Create input polyhedron
    input >> polyhedron;
    if (!input.good()) {
      // Even if the mesh exists, it may not be valid, see
      // <https://github.com/CGAL/cgal/issues/4632>
      std::stringstream msg;
      msg << "Invalid input file \"" << infile << "\"" << std::endl;
      msg << "If this is due to wrong face orientation, retry with the option --reorient \"" << std::endl;
      throw std::runtime_error(msg.str());
    }
  }

  input.close();

  // Create domain
  Mesh_domain cgal_domain(polyhedron);

  // Get sharp features
  // cgal_domain.detect_features();


  // Mesh criteria
  Mesh_criteria criteria(
      CGAL::parameters::edge_size = max_edge_size_at_feature_edges,
      CGAL::parameters::facet_angle = min_facet_angle,
      CGAL::parameters::facet_size = max_radius_surface_delaunay_ball,
      CGAL::parameters::facet_distance = max_facet_distance,
      CGAL::parameters::cell_radius_edge_ratio = max_circumradius_edge_ratio,
      CGAL::parameters::cell_size = max_cell_circumradius);

  // Mesh generation
  if (!verbose) {
    // suppress output
    std::cerr.setstate(std::ios_base::failbit);
  }
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      cgal_domain, criteria,
      lloyd ? CGAL::parameters::lloyd() : CGAL::parameters::no_lloyd(),
      odt ? CGAL::parameters::odt() : CGAL::parameters::no_odt(),
      perturb ? CGAL::parameters::perturb() : CGAL::parameters::no_perturb(),
      exude ?
        CGAL::parameters::exude(
          CGAL::parameters::time_limit = exude_time_limit,
          CGAL::parameters::sliver_bound = exude_sliver_bound
        ) :
        CGAL::parameters::no_exude()
      );
  if (!verbose) {
    std::cerr.clear();
  }

  // Output
  std::ofstream medit_file(outfile);
  c3t3.output_to_medit(medit_file);
  medit_file.close();

  return;
}

}  // namespace pygalmesh
