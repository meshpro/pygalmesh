#include "generate.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/Mesh_domain_with_polyline_features_3.h>
#include <CGAL/make_mesh_3.h>

namespace loom {

typedef CGAL::Mesh_domain_with_polyline_features_3<
  CGAL::Implicit_mesh_domain_3<DomainBase,K>
  >
  Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef Mesh_criteria::Facet_criteria Facet_criteria;
typedef Mesh_criteria::Cell_criteria Cell_criteria;

// Function
K::FT sphere_function1 (const K::Point_3& p) {
  return CGAL::squared_distance(p, K::Point_3(CGAL::ORIGIN))-2;
}
K::FT sphere_function2 (const K::Point_3& p) {
  return CGAL::squared_distance(p, K::Point_3(2, 0, 0))-2;
}
K::FT sphere_function (const K::Point_3& p)
{
  if(sphere_function1(p) < 0 || sphere_function2(p) < 0)
    return -1;
  else
    return 1;
}

void
generate_mesh(
    const std::shared_ptr<loom::DomainBase> & domain,
    const double bounding_sphere_radius,
    const std::string & outfile,
    const std::vector<std::vector<std::vector<double>>> & feature_edges,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude,
    const double boundary_precision,
    const double facet_angle,
    const double facet_size,
    const double facet_distance,
    const double cell_radius_edge_ratio,
    const double cell_size
    )
{
  Mesh_domain cgal_domain(
      *domain,
      K::Sphere_3(CGAL::ORIGIN, bounding_sphere_radius*bounding_sphere_radius),
      boundary_precision
      );

  if (feature_edges.size() > 0) {
    // translate to list of vectors of Point_3
    std::list<std::vector<K::Point_3>> polylines;
    for (const auto & feature_edge: feature_edges) {
      std::vector<K::Point_3> polyline;
      for (const auto & point: feature_edge) {
          polyline.push_back(K::Point_3(point[0], point[1], point[2]));
      }
      polylines.push_back(polyline);
    }
    cgal_domain.add_features(polylines.begin(), polylines.end());
  }

  // Polylines polylines (1);
  // Polyline_3& polyline = polylines.front();
  // for(int i = 0; i < 360; ++i) {
  //   K::Point_3 p(1, std::cos(i*CGAL_PI/180), std::sin(i*CGAL_PI/180));
  //   polyline.push_back(p);
  // }
  // polyline.push_back(polyline.front());
  // cgal_domain.add_features(polylines.begin(), polylines.end());

  // Facet_criteria facet_criteria(facet_angle, facet_size, facet_distance);
  // Cell_criteria cell_criteria(cell_radius_edge_ratio, cell_size);
  // Mesh_criteria criteria(facet_criteria, cell_criteria);

  Mesh_criteria criteria(
      CGAL::parameters::edge_size=0.15,
      CGAL::parameters::facet_angle=25,
      CGAL::parameters::facet_size=0.15,
      CGAL::parameters::cell_radius_edge_ratio=2,
      CGAL::parameters::cell_size=0.15
      );

  // Mesh generation
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      cgal_domain,
      criteria,
      lloyd ? CGAL::parameters::lloyd() : CGAL::parameters::no_lloyd(),
      odt ? CGAL::parameters::odt() : CGAL::parameters::no_odt(),
      perturb ? CGAL::parameters::perturb() : CGAL::parameters::no_perturb(),
      exude ? CGAL::parameters::exude() : CGAL::parameters::no_exude()
      );

  // Output
  std::ofstream medit_file(outfile);
  c3t3.output_to_medit(medit_file);
  medit_file.close();

  return;
}

} // namespace loom
