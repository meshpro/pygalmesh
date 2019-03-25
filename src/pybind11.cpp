#include "domain.hpp"
#include "generate.hpp"
#include "generate_from_off.hpp"
#include "generate_surface_mesh.hpp"
#include "polygon2d.hpp"
#include "primitives.hpp"

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
              py::arg("edge_size") = 0.0
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
        "_generate_mesh", &generate_mesh,
        py::arg("domain"),
        py::arg("outfile"),
        py::arg("feature_edges") = std::vector<std::vector<std::array<double, 3>>>(),
        py::arg("bounding_sphere_radius") = 0.0,
        py::arg("boundary_precision") = 1.0e-4,
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("edge_size") = 0.0,
        py::arg("facet_angle") = 0.0,
        py::arg("facet_size") = 0.0,
        py::arg("facet_distance") = 0.0,
        py::arg("cell_radius_edge_ratio") = 0.0,
        py::arg("cell_size") = 0.0,
        py::arg("verbose") = true
        );
    m.def(
        "_generate_surface_mesh", &generate_surface_mesh,
        py::arg("domain"),
        py::arg("outfile"),
        py::arg("bounding_sphere_radius") = 0.0,
        py::arg("angle_bound") = 0.0,
        py::arg("radius_bound") = 0.0,
        py::arg("distance_bound") = 0.0,
        py::arg("verbose") = true
        );
    m.def(
        "_generate_from_off", &generate_from_off,
        py::arg("infile"),
        py::arg("outfile"),
        py::arg("lloyd") = false,
        py::arg("odt") = false,
        py::arg("perturb") = true,
        py::arg("exude") = true,
        py::arg("edge_size") = 0.0,  // std::numeric_limits<double>::max(),
        py::arg("facet_angle") = 0.0,
        py::arg("facet_size") = 0.0,
        py::arg("facet_distance") = 0.0,
        py::arg("cell_radius_edge_ratio") = 0.0,
        py::arg("cell_size") = 0.0,
        py::arg("verbose") = true
        );
}
