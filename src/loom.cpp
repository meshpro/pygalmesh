#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_to_labeling_function_wrapper.h>
#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>

#include <loom.hpp>

class Function: public std::unary_function<K::Point_3, K::FT>
{
  public:
  typedef K::Point_3 Point;

  explicit Function(std::shared_ptr<loom::PrimitiveBase> fun):
    fun_(fun)
  {
  }

  virtual K::FT operator()(K::Point_3 p) const {
    return (*fun_)(p);
  }

  private:
  std::shared_ptr<loom::PrimitiveBase> fun_;
};

typedef CGAL::Implicit_multi_domain_to_labeling_function_wrapper<Function>
                                                        Function_wrapper;
typedef Function_wrapper::Function_vector Function_vector;
typedef CGAL::Labeled_mesh_domain_3<Function_wrapper, K> Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef Mesh_criteria::Facet_criteria    Facet_criteria;
typedef Mesh_criteria::Cell_criteria     Cell_criteria;


void
generate_mesh(
    const std::shared_ptr<DomainBase> & in,
    const bool lloyd,
    const bool odt,
    const bool perturb,
    const bool exude
    )
{
  Function_vector v;
  for (const auto & primitive: in->get_primitives()) {
      v.push_back(Function(primitive));
  }

  // Domain (Warning: Sphere_3 constructor uses square radius !)
  Mesh_domain domain(
      Function_wrapper(v, in->get_signs()),
      K::Sphere_3(CGAL::ORIGIN, 5.*5.),
      1.0e-4
      );

  // Set mesh criteria
  Facet_criteria facet_criteria(30, 0.2, 0.02); // angle, size, approximation
  Cell_criteria cell_criteria(2., 0.4); // radius-edge ratio, size
  Mesh_criteria criteria(facet_criteria, cell_criteria);

  const auto lloyd_param =
    lloyd ? CGAL::parameters::lloyd() : CGAL::parameters::no_lloyd();
  const auto odt_param =
    odt ? CGAL::parameters::odt() : CGAL::parameters::no_odt();
  const auto perturb_param =
    perturb ? CGAL::parameters::perturb() : CGAL::parameters::no_perturb();
  const auto exude_param =
    exude ? CGAL::parameters::exude() : CGAL::parameters::no_exude();

  // Mesh generation
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
      domain,
      criteria,
      lloyd_param,
      odt_param,
      perturb_param,
      exude_param
      );

  // Output
  std::ofstream medit_file("out.mesh");
  c3t3.output_to_medit(medit_file);

  return;
}
