#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <Eigen/Dense>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

namespace loom {

class DomainBase
{
  public:
  virtual
  K::FT
  operator()(K::Point_3 p) const = 0;

  virtual
  double
  get_bounding_sphere_squared_radius() const = 0;

  virtual
  std::list<std::vector<K::Point_3>>
  get_features() const
  {
    return {};
  };
};

class Rotate: public loom::DomainBase
{
  public:
  Rotate(
      const std::shared_ptr<const loom::DomainBase> & domain,
      const std::vector<double> & axis,
      const double angle
      ):
    domain_(domain),
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

    return domain_->operator()(pt2);
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return domain_->get_bounding_sphere_squared_radius();
  }

  private:
    const std::shared_ptr<const loom::DomainBase> domain_;
    const Eigen::Vector3d normalized_axis_;
    const double sinAngle_;
    const double cosAngle_;
};

class Intersection: public loom::DomainBase
{
  public:
  Intersection(
      std::shared_ptr<const loom::DomainBase> & domain0,
      std::shared_ptr<const loom::DomainBase> & domain1
      ):
    domain0_(domain0),
    domain1_(domain1)
  {
  }

  virtual
  K::FT
  operator()(K::Point_3 p) const
  {
    return ((*domain0_)(p) < 0.0 && (*domain1_)(p) < 0.0) ?
        -1.0 :
        1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return std::min({
        domain0_->get_bounding_sphere_squared_radius(),
        domain1_->get_bounding_sphere_squared_radius()
        });
  }

  private:
    std::shared_ptr<const loom::DomainBase> domain0_;
    std::shared_ptr<const loom::DomainBase> domain1_;
};

class Union: public loom::DomainBase
{
  public:
  explicit Union(
      std::vector<std::shared_ptr<const loom::DomainBase>> & domains
      ):
    domains_(domains)
  {
  }

  virtual
  K::FT
  operator()(K::Point_3 p) const
  {
    for (const auto & domain: domains_) {
      if ((*domain)(p) < 0.0) {
        return -1.0;
      }
    }
    return 1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    double max = 0.0;
    for (const auto & domain: domains_) {
      max = std::max(max, domain->get_bounding_sphere_squared_radius());
    }
    return max;
  }

  private:
    std::vector<std::shared_ptr<const loom::DomainBase>> domains_;
};

class Difference: public loom::DomainBase
{
  public:
  Difference(
      std::shared_ptr<const loom::DomainBase> & domain0,
      std::shared_ptr<const loom::DomainBase> & domain1
      ):
    domain0_(domain0),
    domain1_(domain1)
  {
  }

  virtual
  K::FT
  operator()(K::Point_3 p) const
  {
    return ((*domain0_)(p) < 0.0 || (*domain1_)(p) >= 0.0) ?
        -1.0 :
        1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return domain0_->get_bounding_sphere_squared_radius();
  }

  private:
    std::shared_ptr<const loom::DomainBase> domain0_;
    std::shared_ptr<const loom::DomainBase> domain1_;
};

} // namespace loom
#endif // DOMAIN_HPP
