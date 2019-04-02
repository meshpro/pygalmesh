#define CGAL_MESH_3_VERBOSE 1

#include "generate_periodic.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Periodic_3_mesh_3/IO/File_medit.h>
#include <CGAL/Periodic_3_mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/Mesh_domain_with_polyline_features_3.h>
#include <CGAL/make_periodic_3_mesh_3.h>

namespace pygalmesh {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

// Wrapper for DomainBase for translating to K::Point_3/FT.
class CgalDomainWrapper
{
  public:
  explicit CgalDomainWrapper(const std::shared_ptr<DomainBase> & domain):
    domain_(domain)
  {
  }

  virtual ~CgalDomainWrapper() = default;

  virtual
  K::FT
  operator()(K::Point_3 p) const
  {
    return domain_->eval({p.x(), p.y(), p.z()});
  }

  private:
  const std::shared_ptr<DomainBase> domain_;
};

typedef CGAL::Labeled_mesh_domain_3<K> Periodic_mesh_domain;

// Triangulation
typedef CGAL::Periodic_3_mesh_triangulation_3<Periodic_mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef Mesh_criteria::Facet_criteria Facet_criteria;
typedef Mesh_criteria::Cell_criteria Cell_criteria;

void
generate_periodic_mesh(
    const std::shared_ptr<pygalmesh::DomainBase> & domain,
    const std::string & outfile,
    const double bounding_sphere_radius,
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
  int domain_size = 1;
  K::Iso_cuboid_3 cuboid(0, 0, 0, domain_size, domain_size, domain_size);

  const auto d = CgalDomainWrapper(domain);
  Periodic_mesh_domain cgal_domain =
    Periodic_mesh_domain::create_implicit_mesh_domain(d, cuboid);

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
  C3t3 c3t3 = CGAL::make_periodic_3_mesh_3<C3t3>(
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
  const int number_of_copies_in_output = 4;  // can be 1, 2, 4, or 8
  CGAL::output_periodic_mesh_to_medit(medit_file, c3t3, number_of_copies_in_output);
  medit_file.close();

  return;
}

} // namespace pygalmesh
