#define CGAL_MESH_3_VERBOSE 1

#include "generate_from_inr.hpp"

#include <cassert>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Image_3.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/Mesh_domain_with_polyline_features_3.h>
#include <CGAL/make_mesh_3.h>

namespace pygalmesh {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Labeled_mesh_domain_3<K> Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef Mesh_criteria::Facet_criteria Facet_criteria;
typedef Mesh_criteria::Cell_criteria Cell_criteria;

typedef CGAL::Mesh_constant_domain_field_3<Mesh_domain::R,
                                           Mesh_domain::Index> Sizing_field_cell;

void
generate_from_inr(
    const std::string & inr_filename,
    const std::string & outfile,
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
    const int seed
    )
{
  CGAL::get_default_random() = CGAL::Random(seed);

  CGAL::Image_3 image;
  const bool success = image.read(inr_filename.c_str());
  if (!success) {
    throw "Could not read image file";
  }
  Mesh_domain cgal_domain = Mesh_domain::create_labeled_image_mesh_domain(image);

  Mesh_criteria criteria(
      CGAL::parameters::edge_size=max_edge_size_at_feature_edges,
      CGAL::parameters::facet_angle=min_facet_angle,
      CGAL::parameters::facet_size=max_radius_surface_delaunay_ball,
      CGAL::parameters::facet_distance=max_facet_distance,
      CGAL::parameters::cell_radius_edge_ratio=max_circumradius_edge_ratio,
      CGAL::parameters::cell_size=max_cell_circumradius
      );

  // Mesh generation
  if (!verbose) {
    // suppress output
    std::cerr.setstate(std::ios_base::failbit);
  }
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      cgal_domain,
      criteria,
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


void
generate_from_inr_with_subdomain_sizing(
    const std::string & inr_filename,
    const std::string & outfile,
    const double default_max_cell_circumradius,
    const std::vector<double> & max_cell_circumradiuss,
    const std::vector<int> & cell_labels,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude,
    const double max_edge_size_at_feature_edges,
    const double min_facet_angle,
    const double max_radius_surface_delaunay_ball,
    const double max_facet_distance,
    const double max_circumradius_edge_ratio,
    const double exude_time_limit,
    const double exude_sliver_bound,
    const bool verbose,
    const int seed
    )
{
  CGAL::get_default_random() = CGAL::Random(seed);

  CGAL::Image_3 image;
  const bool success = image.read(inr_filename.c_str());
  if (!success) {
    throw "Could not read image file";
  }
  Mesh_domain cgal_domain = Mesh_domain::create_labeled_image_mesh_domain(image);

  Sizing_field_cell max_cell_circumradius(default_max_cell_circumradius);
  const int ndimensions = 3;
  for(std::vector<double>::size_type i(0); i < max_cell_circumradiuss.size(); ++i)
    max_cell_circumradius.set_size(max_cell_circumradiuss[i], ndimensions, cgal_domain.index_from_subdomain_index(cell_labels[i]));

  Mesh_criteria criteria(
      CGAL::parameters::edge_size=max_edge_size_at_feature_edges,
      CGAL::parameters::facet_angle=min_facet_angle,
      CGAL::parameters::facet_size=max_radius_surface_delaunay_ball,
      CGAL::parameters::facet_distance=max_facet_distance,
      CGAL::parameters::cell_radius_edge_ratio=max_circumradius_edge_ratio,
      CGAL::parameters::cell_size=max_cell_circumradius
      );

  // Mesh generation
  if (!verbose) {
    // suppress output
    std::cerr.setstate(std::ios_base::failbit);
  }
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      cgal_domain,
      criteria,
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

} // namespace pygalmesh
