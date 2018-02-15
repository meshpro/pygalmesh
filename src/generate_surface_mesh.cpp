#define CGAL_SURFACE_MESHER_VERBOSE 1

#include "generate_surface_mesh.hpp"

#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <fstream>
#include <CGAL/IO/Complex_2_in_triangulation_3_file_writer.h>
#include <CGAL/Implicit_surface_3.h>

namespace pygalmesh {

// default triangulation for Surface_mesher
typedef CGAL::Surface_mesh_default_triangulation_3 Tr;
// c2t3
typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;
typedef Tr::Geom_traits GT;

// Wrapper for DomainBase for translating to GT.
class CgalDomainWrapper
{
  public:
  explicit CgalDomainWrapper(const std::shared_ptr<DomainBase> & domain):
    domain_(domain)
  {
  }

  virtual ~CgalDomainWrapper() = default;

  virtual
  GT::FT
  operator()(GT::Point_3 p) const
  {
    return domain_->eval({p.x(), p.y(), p.z()});
  }

  private:
  const std::shared_ptr<DomainBase> domain_;
};

typedef CGAL::Implicit_surface_3<GT, CgalDomainWrapper> Surface_3;

void
generate_surface_mesh(
    const std::shared_ptr<pygalmesh::DomainBase> & domain,
    const std::string & outfile,
    const double bounding_sphere_radius,
    const double angle_bound,
    const double radius_bound,
    const double distance_bound,
    const bool verbose
    )
{
  const double bounding_sphere_radius2 = bounding_sphere_radius > 0 ?
    bounding_sphere_radius*bounding_sphere_radius :
    // add a little wiggle room
    1.01 * domain->get_bounding_sphere_squared_radius();

  Tr tr;  // 3D-Delaunay triangulation
  C2t3 c2t3 (tr);  // 2D-complex in 3D-Delaunay triangulation

  const auto d = CgalDomainWrapper(domain);
  Surface_3 surface(
      d,
      GT::Sphere_3(CGAL::ORIGIN, bounding_sphere_radius2)
      );

  CGAL::Surface_mesh_default_criteria_3<Tr> criteria(
      angle_bound,
      radius_bound,
      distance_bound
      );

  if (!verbose) {
    // suppress output
    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);
  }
  CGAL::make_surface_mesh(
      c2t3,
      surface,
      criteria,
      CGAL::Non_manifold_tag()
      );
  if (!verbose) {
    std::cout.clear();
    std::cerr.clear();
  }

  // Output
  std::ofstream off_file(outfile);
  CGAL::output_surface_facets_to_off(off_file, c2t3);
  off_file.close();

  return;
}

} // namespace pygalmesh
