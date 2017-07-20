#include "domain.hpp"
#include "generate.hpp"
#include "generate_from_off.hpp"
#include "generate_surface_mesh.hpp"
#include "primitives.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace frentos;

PYBIND11_PLUGIN(frentos) {
    py::module m("frentos");

    // Domain transformations
    py::class_<Translate>(m, "Translate")
          .def(py::init<
              const std::shared_ptr<const DomainBase> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Translate::eval)
          .def("translate_features", &Translate::translate_features)
          .def("get_bounding_sphere_squared_radius", &Translate::get_bounding_sphere_squared_radius)
          .def("get_features", &Translate::get_features);

    py::class_<Rotate>(m, "Rotate")
          .def(py::init<
              const std::shared_ptr<const frentos::DomainBase> &,
              const std::array<double, 3> &,
              const double
              >())
          .def("eval", &Rotate::eval)
          .def("rotate", &Rotate::rotate)
          .def("rotate_features", &Rotate::rotate_features)
          .def("get_bounding_sphere_squared_radius", &Rotate::get_bounding_sphere_squared_radius)
          .def("get_features", &Rotate::get_features);

    py::class_<Scale>(m, "Scale")
          .def(py::init<
              std::shared_ptr<const frentos::DomainBase> &,
              const double
              >())
          .def("eval", &Scale::eval)
          .def("scale_features", &Scale::scale_features)
          .def("get_bounding_sphere_squared_radius", &Scale::get_bounding_sphere_squared_radius)
          .def("get_features", &Scale::get_features);

    py::class_<Stretch>(m, "Stretch")
          .def(py::init<
              std::shared_ptr<const frentos::DomainBase> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Stretch::eval)
          .def("stretch_features", &Stretch::stretch_features)
          .def("get_bounding_sphere_squared_radius", &Stretch::get_bounding_sphere_squared_radius)
          .def("get_features", &Stretch::get_features);

    py::class_<Intersection>(m, "Intersection")
          .def(py::init<
              std::vector<std::shared_ptr<const frentos::DomainBase>> &
              >())
          .def("eval", &Intersection::eval)
          .def("get_bounding_sphere_squared_radius", &Intersection::get_bounding_sphere_squared_radius)
          .def("get_features", &Intersection::get_features);

    py::class_<Union>(m, "Union")
          .def(py::init<
              std::vector<std::shared_ptr<const frentos::DomainBase>> &
              >())
          .def("eval", &Union::eval)
          .def("get_bounding_sphere_squared_radius", &Union::get_bounding_sphere_squared_radius)
          .def("get_features", &Union::get_features);

    py::class_<Difference>(m, "Difference")
          .def(py::init<
              std::shared_ptr<const frentos::DomainBase> &,
              std::shared_ptr<const frentos::DomainBase> &
              >())
          .def("eval", &Difference::eval)
          .def("get_bounding_sphere_squared_radius", &Difference::get_bounding_sphere_squared_radius)
          .def("get_features", &Difference::get_features);

    // Primitives
    py::class_<Ball>(m, "Ball")
          .def(py::init<
              const std::array<double, 3> &,
              const double
              >())
          .def("eval", &Ball::eval)
          .def("get_bounding_sphere_squared_radius", &Ball::get_bounding_sphere_squared_radius);

    py::class_<Cuboid>(m, "Cuboid")
          .def(py::init<
              const std::array<double, 3> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Cuboid::eval)
          .def("get_bounding_sphere_squared_radius", &Cuboid::get_bounding_sphere_squared_radius)
          .def("get_features", &Cuboid::get_features);

    py::class_<Ellipsoid>(m, "Ellipsoid")
          .def(py::init<
              const std::array<double, 3> &,
              const double,
              const double,
              const double
              >())
          .def("eval", &Ellipsoid::eval)
          .def("get_bounding_sphere_squared_radius", &Ellipsoid::get_bounding_sphere_squared_radius)
          .def("get_features", &Ellipsoid::get_features);

    py::class_<Cylinder>(m, "Cylinder")
          .def(py::init<
              const double,
              const double,
              const double,
              const double
              >())
          .def("eval", &Cylinder::eval)
          .def("get_bounding_sphere_squared_radius", &Cylinder::get_bounding_sphere_squared_radius)
          .def("get_features", &Cylinder::get_features);

    py::class_<Cone>(m, "Cone")
          .def(py::init<
              const double,
              const double,
              const double
              >())
          .def("eval", &Cone::eval)
          .def("get_bounding_sphere_squared_radius", &Cone::get_bounding_sphere_squared_radius)
          .def("get_features", &Cone::get_features);

    py::class_<Tetrahedron>(m, "Tetrahedron")
          .def(py::init<
              const std::array<double, 3> &,
              const std::array<double, 3> &,
              const std::array<double, 3> &,
              const std::array<double, 3> &
              >())
          .def("eval", &Tetrahedron::eval)
          .def("get_bounding_sphere_squared_radius", &Tetrahedron::get_bounding_sphere_squared_radius)
          .def("get_features", &Tetrahedron::get_features);

    py::class_<Torus>(m, "Torus")
          .def(py::init<
              const double,
              const double
              >())
          .def("eval", &Torus::eval)
          .def("get_bounding_sphere_squared_radius", &Torus::get_bounding_sphere_squared_radius)
          .def("get_features", &Torus::get_features);

    // functions
    m.def("generate_from_off", &generate_from_off);
    m.def("generate_mesh", &generate_mesh);
    m.def("generate_surface_mesh", &generate_surface_mesh);
    return m.ptr();
}
