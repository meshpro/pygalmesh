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

class Translate: public loom::DomainBase
{
  public:
  Translate(
      const std::shared_ptr<const loom::DomainBase> & domain,
      const std::vector<double> & direction
      ):
    domain_(domain),
    direction_(Eigen::Vector3d(direction.data())),
    translated_features_(translate_features(domain->get_features(), direction_))
  {
  }

  virtual ~Translate() = default;

  std::vector<std::vector<std::vector<double>>>
  translate_features(
      const std::vector<std::vector<std::vector<double>>> & features,
      const Eigen::Vector3d & direction
      ) const
  {
    std::vector<std::vector<std::vector<double>>> translated_features;
    for (const auto & feature: features) {
      std::vector<std::vector<double>> translated_feature;
      for (const auto & point: feature) {
        const std::vector<double> translated_point = {
          point[0] + direction[0],
          point[1] + direction[1],
          point[2] + direction[2]
        };
        translated_feature.push_back(translated_point);
      }
      translated_features.push_back(translated_feature);
    }
    return translated_features;
  }

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    const Eigen::Vector3d p_vec(x, y, z);
    const Eigen::Vector3d p2 = p_vec - direction_;
    const auto pt2 = K::Point_3(p2[0], p2[1], p2[2]);
    return domain_->operator()(pt2);
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    const double radius = sqrt(domain_->get_bounding_sphere_squared_radius());
    const double dir_norm = direction_.norm();
    return (radius + dir_norm)*(radius + dir_norm);
  }

  virtual
  std::vector<std::vector<std::vector<double>>>
  get_features() const
  {
    return translated_features_;
  };

  private:
    const std::shared_ptr<const loom::DomainBase> domain_;
    const Eigen::Vector3d direction_;
    const std::vector<std::vector<std::vector<double>>> translated_features_;
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
    cosAngle_(cos(angle)),
    rotated_features_(rotate_features(domain_->get_features()))
  {
  }

  virtual ~Rotate() = default;

  Eigen::Vector3d
  rotate(
      const Eigen::Vector3d & vec,
      const Eigen::Vector3d & axis,
      const double sinAngle,
      const double cosAngle
      ) const
  {
    // Rotate a vector `v` by the angle `theta` in the plane perpendicular
    // to the axis given by `u`.
    // Refer to
    // http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    //
    // cos(theta) * I * v
    // + sin(theta) u\cross v
    // + (1-cos(theta)) (u*u^T) * v
    return cosAngle * vec
      + sinAngle * axis.cross(vec)
      + (1.0-cosAngle) * axis.dot(vec) * axis;
  }

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    // rotate with negative angle
    const auto p2 = rotate(
        Eigen::Vector3d(x, y, z),
        normalized_axis_,
        -sinAngle_,
        cosAngle_
        );
    const auto pt2 = K::Point_3(p2[0], p2[1], p2[2]);
    return domain_->operator()(pt2);
  }

  std::vector<std::vector<std::vector<double>>>
  rotate_features(
      const std::vector<std::vector<std::vector<double>>> & features
      ) const
  {
    std::vector<std::vector<std::vector<double>>> rotated_features;
    for (const auto & feature: features) {
      std::vector<std::vector<double>> rotated_feature;
      for (const auto & point: feature) {
        const auto p2 = rotate(
            Eigen::Vector3d(point.data()),
            normalized_axis_,
            sinAngle_,
            cosAngle_
            );
        rotated_feature.push_back({p2[0], p2[1], p2[2]});
      }
      rotated_features.push_back(rotated_feature);
    }
    return rotated_features;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return domain_->get_bounding_sphere_squared_radius();
  }

  virtual
  std::vector<std::vector<std::vector<double>>>
  get_features() const
  {
    return rotated_features_;
  };

  private:
    const std::shared_ptr<const loom::DomainBase> domain_;
    const Eigen::Vector3d normalized_axis_;
    const double sinAngle_;
    const double cosAngle_;
    const std::vector<std::vector<std::vector<double>>> rotated_features_;
};

class Scale: public loom::DomainBase
{
  public:
  Scale(
      std::shared_ptr<const loom::DomainBase> & domain,
      const double alpha
      ):
    domain_(domain),
    alpha_(alpha),
    scaled_features_(scale_features(domain_->get_features()))
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

  std::vector<std::vector<std::vector<double>>>
  scale_features(
      const std::vector<std::vector<std::vector<double>>> & features
      ) const
  {
    std::vector<std::vector<std::vector<double>>> scaled_features;
    for (const auto & feature: features) {
      std::vector<std::vector<double>> scaled_feature;
      for (const auto & point: feature) {
        scaled_feature.push_back({
            alpha_ * point[0],
            alpha_ * point[1],
            alpha_ * point[2]
            });
      }
      scaled_features.push_back(scaled_feature);
    }
    return scaled_features;
  }

  virtual
  std::vector<std::vector<std::vector<double>>>
  get_features() const
  {
    return scaled_features_;
  };

  private:
    std::shared_ptr<const loom::DomainBase> domain_;
    const double alpha_;
    const std::vector<std::vector<std::vector<double>>> scaled_features_;
};

class Stretch: public loom::DomainBase
{
  public:
  Stretch(
      std::shared_ptr<const loom::DomainBase> & domain,
      const std::vector<double> & direction
      ):
    domain_(domain),
    normalized_direction_(Eigen::Vector3d(direction.data()).normalized()),
    alpha_(Eigen::Vector3d(direction.data()).norm())
  {
    assert(alpha_ > 0.0);
  }

  virtual ~Stretch() = default;

  virtual
  double
  eval(const double x, const double y, const double z) const
  {
    const Eigen::Vector3d v(x, y, z);
    const double beta = normalized_direction_.dot(v);
    // scale the component of normalized_direction_ by 1/alpha_
    const auto v2 = beta/alpha_ * normalized_direction_
       + (v - beta * normalized_direction_);
    return domain_->eval(v2[0], v2[1], v2[2]);
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return alpha_*alpha_ * domain_->get_bounding_sphere_squared_radius();
  }

  private:
    std::shared_ptr<const loom::DomainBase> domain_;
    const Eigen::Vector3d normalized_direction_;
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
