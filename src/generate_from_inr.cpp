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
    const double edge_size,
    const double facet_angle,
    const double facet_size,
    const double facet_distance,
    const double cell_radius_edge_ratio,
    const double cell_size,
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
      CGAL::parameters::edge_size=edge_size,
      CGAL::parameters::facet_angle=facet_angle,
      CGAL::parameters::facet_size=facet_size,
      CGAL::parameters::facet_distance=facet_distance,
      CGAL::parameters::cell_radius_edge_ratio=cell_radius_edge_ratio,
      CGAL::parameters::cell_size=cell_size
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
      exude ? CGAL::parameters::exude() : CGAL::parameters::no_exude()
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
    const double default_cell_size,
    const std::vector<double> & cell_sizes,
    const std::vector<int> & cell_labels,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude,
    const double edge_size,
    const double facet_angle,
    const double facet_size,
    const double facet_distance,
    const double cell_radius_edge_ratio,
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

  Sizing_field_cell cell_size(default_cell_size);
  const int ndimensions = 3;
  for(std::vector<double>::size_type i(0); i < cell_sizes.size(); ++i)
    cell_size.set_size(cell_sizes[i], ndimensions, cgal_domain.index_from_subdomain_index(cell_labels[i]));

  Mesh_criteria criteria(
      CGAL::parameters::edge_size=edge_size,
      CGAL::parameters::facet_angle=facet_angle,
      CGAL::parameters::facet_size=facet_size,
      CGAL::parameters::facet_distance=facet_distance,
      CGAL::parameters::cell_radius_edge_ratio=cell_radius_edge_ratio,
      CGAL::parameters::cell_size=cell_size
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
      exude ? CGAL::parameters::exude() : CGAL::parameters::no_exude()
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
