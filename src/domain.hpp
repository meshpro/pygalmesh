#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include "primitives.hpp"

#include <Eigen/Dense>

namespace loom {

class RotatedPrimitive: public PrimitiveBase
{
  public:
  RotatedPrimitive(
      const std::shared_ptr<const PrimitiveBase> & primitive,
      const std::vector<double> & axis,
      const double angle
      ):
    primitive_(primitive),
    normalized_axis_(Eigen::Vector3d(axis.data()).normalized()),
    sinAngle_(sin(angle)),
    cosAngle_(cos(angle))
  {
  }

  virtual
  K::FT
  operator()(K::Point_3 p) const
  {
    // Rotate a vector `v` by the angle `theta` in the plane perpendicular
    // to the axis given by `u`.
    // Refer to
    // http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    const Eigen::Vector3d p_vec(p.x(), p.y(), p.z());

    // cos(-theta) * I * v
    // + sin(-theta) u\cross v
    // + (1-cos(-theta)) (u*u^T) * v
    const Eigen::Vector3d p2 =
      cosAngle_ * p_vec
      - sinAngle_ * normalized_axis_.cross(p_vec)
      + (1.0-cosAngle_) * normalized_axis_.dot(p_vec) * normalized_axis_;

    const auto pt2 = K::Point_3(p2[0], p2[1], p2[2]);

    return primitive_->operator()(pt2);
  }

  virtual std::vector<std::vector<CGAL::Sign>> get_signs() const
  {
    return primitive_->get_signs();
  }

  private:
    const std::shared_ptr<const PrimitiveBase> primitive_;
    const Eigen::Vector3d normalized_axis_;
    const double sinAngle_;
    const double cosAngle_;
};

class Rotate: public DomainBase
{
  public:
  Rotate(
      std::shared_ptr<const DomainBase> & domain,
      const std::vector<double> & axis,
      const double angle
      ):
    rotated_primitives_(
        create_rotated_primitives_(domain->get_primitives(), axis, angle)
        ),
    // change in the signs
    signs_(domain->get_signs())
  {
  }

  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  create_rotated_primitives_(
      std::vector<std::shared_ptr<const loom::PrimitiveBase>> primitives,
      const std::vector<double> & axis,
      const double angle
      )
  {
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> rotated_primitives = {};
    for (const auto & primitive: primitives) {
      rotated_primitives.push_back(
          std::make_shared<RotatedPrimitive>(primitive, axis, angle)
          );
    }
    return rotated_primitives;
  }

  virtual
  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  get_primitives() const
  {
    return rotated_primitives_;
  }

  virtual
  std::vector<std::vector<CGAL::Sign>>
  get_signs() const
  {
    return signs_;
  }

  private:
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> rotated_primitives_;
    std::vector<std::vector<CGAL::Sign>> signs_;
};

} // namespace loom
#endif // DOMAIN_HPP
