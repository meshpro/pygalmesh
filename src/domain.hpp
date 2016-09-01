#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <Eigen/Dense>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

namespace loom {

class DomainBase
{
  public:

  virtual ~DomainBase() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const = 0;

  virtual
  K::FT
  operator()(K::Point_3 p) const
  {
    return this->eval(p.x(), p.y(), p.z());
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const = 0;

  virtual
  std::vector<std::vector<std::vector<double>>>
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

  virtual ~Rotate() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    // Rotate a vector `v` by the angle `theta` in the plane perpendicular
    // to the axis given by `u`.
    // Refer to
    // http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    const Eigen::Vector3d p_vec(x, y, z);

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

class Scale: public loom::DomainBase
{
  public:
  Scale(
      std::shared_ptr<const loom::DomainBase> & domain,
      const double alpha
      ):
    domain_(domain),
    alpha_(alpha)
  {
    assert(alpha_ > 0.0);
  }

  virtual ~Scale() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    return domain_->eval(x/alpha_, y/alpha_, z/alpha_);
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return alpha_*alpha_ * domain_->get_bounding_sphere_squared_radius();
  }

  private:
    std::shared_ptr<const loom::DomainBase> domain_;
    const double alpha_;
};

class Intersection: public loom::DomainBase
{
  public:
  explicit Intersection(
      std::vector<std::shared_ptr<const loom::DomainBase>> & domains
      ):
    domains_(domains)
  {
  }

  virtual ~Intersection() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    for (const auto & domain: domains_) {
      if (domain->eval(x, y, z) > 0.0) {
        return 1.0;
      }
    }
    return -1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    double min = std::numeric_limits<double>::max();
    for (const auto & domain: domains_) {
      min = std::min(min, domain->get_bounding_sphere_squared_radius());
    }
    return min;
  }

  private:
    std::vector<std::shared_ptr<const loom::DomainBase>> domains_;
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

  virtual ~Union() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    for (const auto & domain: domains_) {
      if (domain->eval(x, y, z) < 0.0) {
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

  virtual ~Difference() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    return (domain0_->eval(x, y, z) < 0.0 && domain1_->eval(x, y, z) >= 0.0) ?
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
