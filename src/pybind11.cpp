#include "domain.hpp"
#include "generate.hpp"
#include "generate_2d.hpp"
#include "generate_from_off.hpp"
#include "generate_from_inr.hpp"
#include "remesh_surface.hpp"
#include "generate_periodic.hpp"
#include "generate_surface_mesh.hpp"
#include "polygon2d.hpp"
#include "primitives.hpp"
#include "sizing_field.hpp"

#include <CGAL/version.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pygalmesh;


// https://pybind11.readthedocs.io/en/stable/advanced/classes.html#overriding-virtual-functions-in-python
class PyDomainBase: public DomainBase {
public:
    using DomainBase::DomainBase;

    double
    eval(const std::array<double, 3> & x) const override {
      PYBIND11_OVERLOAD_PURE(double, DomainBase, eval, x);
    }

    double
    get_bounding_sphere_squared_radius() const override {
      PYBIND11_OVERLOAD_PURE(double, DomainBase, get_bounding_sphere_squared_radius);
    }

    // std::vector<std::vector<std::array<double, 3>>>
    // get_features() const override {
    //   PYBIND11_OVERLOAD(
    //       std::vector<std::vector<std::array<double, 3>>>,
    //       DomainBase,
    //       get_features,
    //       0.0
    //       );
    // }
};


// https://pybind11.readthedocs.io/en/stable/advanced/classes.html#overriding-virtual-functions-in-python
class PySizingFieldBase: public SizingFieldBase {
public:
    using SizingFieldBase::SizingFieldBase;

    double
    eval(const std::array<double, 3> & x) const override {
      PYBIND11_OVERLOAD_PURE(double, SizingFieldBase, eval, x);
    }
};


PYBIND11_MODULE(_pygalmesh, m) {
    // m.doc() = "documentation string";

    // Domain base.
    // shared_ptr b/c of
    // <https://github.com/pybind/pybind11/issues/956#issuecomment-317022720>
    py::class_<DomainBase, PyDomainBase, std::shared_ptr<DomainBase>>(m, "DomainBase")
      .def(py::init<>())
      .def("eval", &DomainBase::eval)
      .def("get_bounding_sphere_squared_radius", &DomainBase::get_bounding_sphere_squared_radius)
      .def("get_features", &DomainBase::get_features);

    // Sizing field base.
    // shared_ptr b/c of
    // <https://github.com/pybind/pybind11/issues/956#issuecomment-317022720>
    py::class_<SizingFieldBase, PySizingFieldBase, std::shared_ptr<SizingFieldBase>>(m, "SizingFieldBase")
      .def(py::init<>())
      .def("eval", &SizingFieldBase::eval);

    // Domain transformations
    py::class_<Translate, DomainBase, std::shared_ptr<Translate>>(m, "Translate")
          .def(py::init<
              const std::shared_ptr<const DomainBase> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Translate::eval)
          .def("translate_features", &Translate::translate_features)
          .def("get_bounding_sphere_squared_radius", &Translate::get_bounding_sphere_squared_radius)
          .def("get_features", &Translate::get_features);

    py::class_<Rotate, DomainBase, std::shared_ptr<Rotate>>(m, "Rotate")
          .def(py::init<
              const std::shared_ptr<const pygalmesh::DomainBase> &,
              const std::array<double, 3> &,
              const double
              >())
          .def("eval", &Rotate::eval)
          .def("rotate", &Rotate::rotate)
          .def("rotate_features", &Rotate::rotate_features)
          .def("get_bounding_sphere_squared_radius", &Rotate::get_bounding_sphere_squared_radius)
          .def("get_features", &Rotate::get_features);

    py::class_<Scale, DomainBase, std::shared_ptr<Scale>>(m, "Scale")
          .def(py::init<
              std::shared_ptr<const pygalmesh::DomainBase> &,
              const double
              >())
          .def("eval", &Scale::eval)
          .def("scale_features", &Scale::scale_features)
          .def("get_bounding_sphere_squared_radius", &Scale::get_bounding_sphere_squared_radius)
          .def("get_features", &Scale::get_features);

    py::class_<Stretch, DomainBase, std::shared_ptr<Stretch>>(m, "Stretch")
          .def(py::init<
              std::shared_ptr<const pygalmesh::DomainBase> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Stretch::eval)
          .def("stretch_features", &Stretch::stretch_features)
          .def("get_bounding_sphere_squared_radius", &Stretch::get_bounding_sphere_squared_radius)
          .def("get_features", &Stretch::get_features);

    py::class_<Intersection, DomainBase, std::shared_ptr<Intersection>>(m, "Intersection")
          .def(py::init<
              std::vector<std::shared_ptr<const pygalmesh::DomainBase>> &
              >())
          .def("eval", &Intersection::eval)
          .def("get_bounding_sphere_squared_radius", &Intersection::get_bounding_sphere_squared_radius)
          .def("get_features", &Intersection::get_features);

    py::class_<Union, DomainBase, std::shared_ptr<Union>>(m, "Union")
          .def(py::init<
              std::vector<std::shared_ptr<const pygalmesh::DomainBase>> &
              >())
          .def("eval", &Union::eval)
          .def("get_bounding_sphere_squared_radius", &Union::get_bounding_sphere_squared_radius)
          .def("get_features", &Union::get_features);

    py::class_<Difference, DomainBase, std::shared_ptr<Difference>>(m, "Difference")
          .def(py::init<
              std::shared_ptr<const pygalmesh::DomainBase> &,
              std::shared_ptr<const pygalmesh::DomainBase> &
              >())
          .def("eval", &Difference::eval)
          .def("get_bounding_sphere_squared_radius", &Difference::get_bounding_sphere_squared_radius)
          .def("get_features", &Difference::get_features);

    // Primitives
    py::class_<Ball, DomainBase, std::shared_ptr<Ball>>(m, "Ball")
          .def(py::init<
              const std::array<double, 3> &,
              const double
              >())
          .def("eval", &Ball::eval)
          .def("get_bounding_sphere_squared_radius", &Ball::get_bounding_sphere_squared_radius);

    py::class_<Cuboid, DomainBase, std::shared_ptr<Cuboid>>(m, "Cuboid")
          .def(py::init<
              const std::array<double, 3> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Cuboid::eval)
          .def("get_bounding_sphere_squared_radius", &Cuboid::get_bounding_sphere_squared_radius)
          .def("get_features", &Cuboid::get_features);

    py::class_<Ellipsoid, DomainBase, std::shared_ptr<Ellipsoid>>(m, "Ellipsoid")
          .def(py::init<
              const std::array<double, 3> &,
              const double,
              const double,
              const double
              >())
          .def("eval", &Ellipsoid::eval)
          .def("get_bounding_sphere_squared_radius", &Ellipsoid::get_bounding_sphere_squared_radius)
          .def("get_features", &Ellipsoid::get_features);

    py::class_<Cylinder, DomainBase, std::shared_ptr<Cylinder>>(m, "Cylinder")
          .def(py::init<
              const double,
              const double,
              const double,
              const double
              >())
          .def("eval", &Cylinder::eval)
          .def("get_bounding_sphere_squared_radius", &Cylinder::get_bounding_sphere_squared_radius)
          .def("get_features", &Cylinder::get_features);

    py::class_<Cone, DomainBase, std::shared_ptr<Cone>>(m, "Cone")
          .def(py::init<
              const double,
              const double,
              const double
              >())
          .def("eval", &Cone::eval)
          .def("get_bounding_sphere_squared_radius", &Cone::get_bounding_sphere_squared_radius)
          .def("get_features", &Cone::get_features);

    py::class_<Tetrahedron, DomainBase, std::shared_ptr<Tetrahedron>>(m, "Tetrahedron")
          .def(py::init<
              const std::array<double, 3> &,
              const std::array<double, 3> &,
              const std::array<double, 3> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Tetrahedron::eval)
          .def("get_bounding_sphere_squared_radius", &Tetrahedron::get_bounding_sphere_squared_radius)
          .def("get_features", &Tetrahedron::get_features);

    py::class_<Torus, DomainBase, std::shared_ptr<Torus>>(m, "Torus")
          .def(py::init<
              const double,
              const double
              >())
          .def("eval", &Torus::eval)
          .def("get_bounding_sphere_squared_radius", &Torus::get_bounding_sphere_squared_radius)
          .def("get_features", &Torus::get_features);

    py::class_<HalfSpace, DomainBase, std::shared_ptr<HalfSpace>>(m, "HalfSpace")
          .def(py::init<
              const std::array<double, 3> &,
              const double,
              const double
              >())
          .def("eval", &HalfSpace::eval)
          .def("get_bounding_sphere_squared_radius", &HalfSpace::get_bounding_sphere_squared_radius);

    // polygon2d
    py::class_<Polygon2D, std::shared_ptr<Polygon2D>>(m, "Polygon2D")
          .def(py::init<
              const std::vector<std::array<double, 2>> &
              >())
          .def("vector_to_cgal_points", &Polygon2D::vector_to_cgal_points)
          .def("is_inside", &Polygon2D::is_inside);

    py::class_<Extrude, DomainBase, std::shared_ptr<Extrude>>(m, "Extrude")
          .def(py::init<
              const std::shared_ptr<pygalmesh::Polygon2D> &,
              const std::array<double, 3> &,
              const double,
              const double
              >(),
              py::arg("poly"),
              py::arg("direction"),
              py::arg("alpha") = 0.0,
              py::arg("max_edge_size_at_feature_edges") = 0.0
              )
          .def("eval", &Extrude::eval)
          .def("get_bounding_sphere_squared_radius", &Extrude::get_bounding_sphere_squared_radius)
          .def("get_features", &Extrude::get_features);

    py::class_<ring_extrude, DomainBase, std::shared_ptr<ring_extrude>>(m, "RingExtrude")
          .def(py::init<
              const std::shared_ptr<pygalmesh::Polygon2D> &,
              const double
              >())
          .def("eval", &ring_extrude::eval)
          .def("get_bounding_sphere_squared_radius", &ring_extrude::get_bounding_sphere_squared_radius)
          .def("get_features", &ring_extrude::get_features);

    // functions
    m.def(
        "_generate_2d", &generate_2d,
        py::arg("points"),
        py::arg("constraints"),
        py::arg("max_circumradius_shortest_edge_ratio") = 1.41421356237,
        py::arg("max_edge_size") = 0.0,
        py::arg("num_lloyd_steps") = 0
        );
    m.def(
        "_generate_mesh", &generate_mesh,
        py::arg("domain"),
        py::arg("outfile"),
        py::arg("extra_feature_edges") = std::vector<std::vector<std::array<double, 3>>>(),
        py::arg("bounding_sphere_radius") = 0.0,
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("max_edge_size_at_feature_edges_value") = 0.0,
        py::arg("max_edge_size_at_feature_edges_field") = nullptr,
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball_value") = 0.0,
        py::arg("max_radius_surface_delaunay_ball_field") = nullptr,
        py::arg("max_facet_distance_value") = 0.0,
        py::arg("max_facet_distance_field") = nullptr,
        py::arg("max_circumradius_edge_ratio") = 0.0,
        py::arg("max_cell_circumradius_value") = 0.0,
        py::arg("max_cell_circumradius_field") = nullptr,
        py::arg("exude_time_limit") = 0.0,
        py::arg("exude_sliver_bound") = 0.0,
        py::arg("relative_error_bound") = 1e-3,
        py::arg("verbose") = true,
        py::arg("seed") = 0
        );
    m.def(
        "_generate_periodic_mesh", &generate_periodic_mesh,
        py::arg("domain"),
        py::arg("outfile"),
        py::arg("bounding_cuboid"),
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("max_edge_size_at_feature_edges") = 0.0,
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball") = 0.0,
        py::arg("max_facet_distance") = 0.0,
        py::arg("max_circumradius_edge_ratio") = 0.0,
        py::arg("max_cell_circumradius") = 0.0,
        py::arg("number_of_copies_in_output") = 1,
        py::arg("verbose") = true,
        py::arg("seed") = 0
        );
    m.def(
        "_generate_surface_mesh", &generate_surface_mesh,
        py::arg("domain"),
        py::arg("outfile"),
        py::arg("bounding_sphere_radius") = 0.0,
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball") = 0.0,
        py::arg("max_facet_distance") = 0.0,
        py::arg("verbose") = true,
        py::arg("seed") = 0
        );
    m.def(
        "_generate_from_off", &generate_from_off,
        py::arg("infile"),
        py::arg("outfile"),
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("max_edge_size_at_feature_edges") = 0.0,  // std::numeric_limits<double>::max(),
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball") = 0.0,
        py::arg("max_facet_distance") = 0.0,
        py::arg("max_circumradius_edge_ratio") = 0.0,
        py::arg("max_cell_circumradius") = 0.0,
        py::arg("exude_time_limit") = 0.0,
        py::arg("exude_sliver_bound") = 0.0,
        py::arg("verbose") = true,
        py::arg("reorient") = false,
        py::arg("seed") = 0
        );
    m.def(
        "_generate_from_inr", &generate_from_inr,
        py::arg("inr_filename"),
        py::arg("outfile"),
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("with_features") = false,
        py::arg("max_edge_size_at_feature_edges") = 0.0,
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball") = 0.0,
        py::arg("max_facet_distance") = 0.0,
        py::arg("max_circumradius_edge_ratio") = 0.0,
        py::arg("max_cell_circumradius") = 0.0,
        py::arg("exude_time_limit") = 0.0,
        py::arg("exude_sliver_bound") = 0.0,
        py::arg("relative_error_bound") = 1e-3,
        py::arg("verbose") = true,
        py::arg("seed") = 0
        );
    m.def(
        "_generate_from_inr_with_subdomain_sizing", &generate_from_inr_with_subdomain_sizing,
        py::arg("inr_filename"),
        py::arg("outfile"),
        py::arg("default_max_cell_circumradius"),
        py::arg("max_cell_circumradiuss"),
        py::arg("cell_labels"),
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("max_edge_size_at_feature_edges") = 0.0,
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball") = 0.0,
        py::arg("max_facet_distance") = 0.0,
        py::arg("max_circumradius_edge_ratio") = 0.0,
        py::arg("exude_time_limit") = 0.0,
        py::arg("exude_sliver_bound") = 0.0,
        py::arg("relative_error_bound") = 1e-3,
        py::arg("verbose") = true,
        py::arg("seed") = 0
        );
    m.def(
        "_remesh_surface", &remesh_surface,
        py::arg("infile"),
        py::arg("outfile"),
        py::arg("max_edge_size_at_feature_edges") = 0.0,
        py::arg("min_facet_angle") = 0.0,
        py::arg("max_radius_surface_delaunay_ball") = 0.0,
        py::arg("max_facet_distance") = 0.0,
        py::arg("verbose") = true,
        py::arg("seed") = 0
        );
    m.attr("_CGAL_VERSION_STR") = CGAL_VERSION_STR;
}
