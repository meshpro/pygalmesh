#define CGAL_MESH_3_VERBOSE 1

#include "generate.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/Mesh_domain_with_polyline_features_3.h>
#include <CGAL/make_mesh_3.h>

namespace pygalmesh {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Mesh_domain_with_polyline_features_3<CGAL::Labeled_mesh_domain_3<K>> Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef Mesh_criteria::Facet_criteria Facet_criteria;
typedef Mesh_criteria::Cell_criteria Cell_criteria;

// translate vector<vector<array<double, 3>> to list<vector<Point_3>>
std::list<std::vector<K::Point_3>>
translate_feature_edges(
    const std::vector<std::vector<std::array<double, 3>>> & feature_edges
    )
{
  std::list<std::vector<K::Point_3>> polylines;
  for (const auto & feature_edge: feature_edges) {
    std::vector<K::Point_3> polyline;
    for (const auto & point: feature_edge) {
      polyline.push_back(K::Point_3(point[0], point[1], point[2]));
    }
    polylines.push_back(polyline);
  }
  return polylines;
}

void
generate_mesh(
    const std::shared_ptr<pygalmesh::DomainBase> & domain,
    const std::string & outfile,
    const std::vector<std::vector<std::array<double, 3>>> & feature_edges,
    const double bounding_sphere_radius,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude,
    //
    const double edge_size_value,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & edge_size_field,
    //
    const double facet_angle,
    //
    const double facet_size_value,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & facet_size_field,
    //
    const double facet_distance_value,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & facet_distance_field,
    //
    const double cell_radius_edge_ratio,
    //
    const double cell_size_value,
    const std::shared_ptr<pygalmesh::SizingFieldBase> & cell_size_field,
    //
    const bool verbose,
    const int seed
    )
{
  CGAL::get_default_random() = CGAL::Random(seed);

  const double bounding_sphere_radius2 = bounding_sphere_radius > 0 ?
    bounding_sphere_radius*bounding_sphere_radius :
    // some wiggle room
    1.01 * domain->get_bounding_sphere_squared_radius();

  // wrap domain
  const auto d = [&](K::Point_3 p) {
    return domain->eval({p.x(), p.y(), p.z()});
  };

  Mesh_domain cgal_domain = Mesh_domain::create_implicit_mesh_domain(
       d,
       K::Sphere_3(CGAL::ORIGIN, bounding_sphere_radius2)
       );

  // cgal_domain.detect_features();

  const auto native_features = translate_feature_edges(domain->get_features());
  cgal_domain.add_features(native_features.begin(), native_features.end());

  const auto polylines = translate_feature_edges(feature_edges);
  cgal_domain.add_features(polylines.begin(), polylines.end());

  // perhaps there's a more elegant solution here
  // see <https://github.com/CGAL/cgal/issues/4145>
  if (!verbose) {
    // suppress output
    std::cerr.setstate(std::ios_base::failbit);
  }

  std::cout << "edge_size       " << edge_size_value << " " << edge_size_field << std::endl;
  std::cout << "facet_size      " << facet_size_value << " " << facet_size_field << std::endl;
  std::cout << "facet_distance  " << facet_distance_value << " " << facet_distance_field << std::endl;
  std::cout << "cell_size       " << cell_size_value << " " << cell_size_field << std::endl;

  // Wrap into CGAL-conform functions
  std::function<double(K::Point_3, const int, const Mesh_domain::Index&)> edge_size;
  if (edge_size_field) {
    edge_size = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return edge_size_field->eval({p.x(), p.y(), p.z()});
    };
  } else {
    edge_size = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return edge_size_value;
    };
  }
  //
  std::function<double(K::Point_3, const int, const Mesh_domain::Index&)> facet_size;
  if (facet_size_field) {
    std::cout << "facet size: field chosen, value  " << facet_size_field << std::endl;
    facet_size = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return facet_size_field->eval({p.x(), p.y(), p.z()});
    };
  } else {
    std::cout << "facet size: constant chosen, value  " << facet_size_value << std::endl;
    facet_size = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return facet_size_value;
    };
  }
  //
  std::function<double(K::Point_3, const int, const Mesh_domain::Index&)> facet_distance;
  if (facet_distance_field) {
    std::cout << "facet distance: field chosen, value  " << facet_size_field << std::endl;
    facet_distance = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return facet_distance_field->eval({p.x(), p.y(), p.z()});
    };
  } else {
    std::cout << "facet distance: constant chosen, value  " << facet_size_value << std::endl;
    facet_distance = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return facet_distance_value;
    };
  }
  //
  std::function<double(K::Point_3, const int, const Mesh_domain::Index&)> cell_size;
  if (cell_size_field) {
    cell_size = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return cell_size_field->eval({p.x(), p.y(), p.z()});
    };
  } else {
    cell_size = [&](K::Point_3 p, const int, const Mesh_domain::Index&) {
      return cell_size_value;
    };
  }

  const auto criteria = Mesh_criteria(
      CGAL::parameters::edge_size=edge_size, // ok
      CGAL::parameters::facet_angle=facet_angle, // ok
      // CGAL::parameters::facet_size=facet_size, // NOT ok
      // CGAL::parameters::facet_distance=facet_distance, NOT ok
      CGAL::parameters::facet_size=facet_size,
      CGAL::parameters::cell_radius_edge_ratio=cell_radius_edge_ratio, // ok
      //
      CGAL::parameters::cell_size=cell_size // ok
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
