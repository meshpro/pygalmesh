#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <Eigen/Dense>
#include <array>
#include <limits>
#include <memory>
#include <vector>

namespace pygalmesh {

class DomainBase
{
  public:

  virtual ~DomainBase() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const = 0;

  virtual
  double
  get_bounding_sphere_squared_radius() const = 0;

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    return {};
  };
};

class Translate: public pygalmesh::DomainBase
{
  public:
  Translate(
      const std::shared_ptr<const pygalmesh::DomainBase> & domain,
      const std::array<double, 3> & direction
      ):
    domain_(domain),
    direction_(Eigen::Vector3d(direction.data())),
    translated_features_(translate_features(domain->get_features(), direction_))
  {
  }

  virtual ~Translate() = default;

  std::vector<std::vector<std::array<double, 3>>>
  translate_features(
      const std::vector<std::vector<std::array<double, 3>>> & features,
      const Eigen::Vector3d & direction
      ) const
  {
    std::vector<std::vector<std::array<double, 3>>> translated_features;
    for (const auto & feature: features) {
      std::vector<std::array<double, 3>> translated_feature;
      for (const auto & point: feature) {
        const std::array<double, 3> translated_point = {
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
  eval(const std::array<double, 3> & x) const
  {
    const std::array<double, 3> d = {
      x[0] - direction_[0],
      x[1] - direction_[1],
      x[2] - direction_[2]
    };
    return domain_->eval(d);
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
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    return translated_features_;
  };

  private:
    const std::shared_ptr<const pygalmesh::DomainBase> domain_;
    const Eigen::Vector3d direction_;
    const std::vector<std::vector<std::array<double, 3>>> translated_features_;
};

class Rotate: public pygalmesh::DomainBase
{
  public:
  Rotate(
      const std::shared_ptr<const pygalmesh::DomainBase> & domain,
      const std::array<double, 3> & axis,
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
  eval(const std::array<double, 3> & x) const
  {
    // rotate with negative angle
    const auto p2 = rotate(
        Eigen::Vector3d(x.data()),
        normalized_axis_,
        -sinAngle_,
        cosAngle_
        );
    return domain_->eval({p2[0], p2[1], p2[2]});
  }

  std::vector<std::vector<std::array<double, 3>>>
  rotate_features(
      const std::vector<std::vector<std::array<double, 3>>> & features
      ) const
  {
    std::vector<std::vector<std::array<double, 3>>> rotated_features;
    for (const auto & feature: features) {
      std::vector<std::array<double, 3>> rotated_feature;
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
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    return rotated_features_;
  };

  private:
    const std::shared_ptr<const pygalmesh::DomainBase> domain_;
    const Eigen::Vector3d normalized_axis_;
    const double sinAngle_;
    const double cosAngle_;
    const std::vector<std::vector<std::array<double, 3>>> rotated_features_;
};

class Scale: public pygalmesh::DomainBase
{
  public:
  Scale(
      std::shared_ptr<const pygalmesh::DomainBase> & domain,
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
  eval(const std::array<double, 3> & x) const
  {
    return domain_->eval({x[0]/alpha_, x[1]/alpha_, x[2]/alpha_});
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return alpha_*alpha_ * domain_->get_bounding_sphere_squared_radius();
  }

  std::vector<std::vector<std::array<double, 3>>>
  scale_features(
      const std::vector<std::vector<std::array<double, 3>>> & features
      ) const
  {
    std::vector<std::vector<std::array<double, 3>>> scaled_features;
    for (const auto & feature: features) {
      std::vector<std::array<double, 3>> scaled_feature;
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
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    return scaled_features_;
  };

  private:
    std::shared_ptr<const pygalmesh::DomainBase> domain_;
    const double alpha_;
    const std::vector<std::vector<std::array<double, 3>>> scaled_features_;
};

class Stretch: public pygalmesh::DomainBase
{
  public:
  Stretch(
      std::shared_ptr<const pygalmesh::DomainBase> & domain,
      const std::array<double, 3> & direction
      ):
    domain_(domain),
    normalized_direction_(Eigen::Vector3d(direction.data()).normalized()),
    alpha_(Eigen::Vector3d(direction.data()).norm()),
    stretched_features_(stretch_features(domain_->get_features()))
  {
    assert(alpha_ > 0.0);
  }

  virtual ~Stretch() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const
  {
    const Eigen::Vector3d v(x.data());
    const double beta = normalized_direction_.dot(v);
    // scale the component of normalized_direction_ by 1/alpha_
    const auto v2 = beta/alpha_ * normalized_direction_
       + (v - beta * normalized_direction_);
    return domain_->eval({v2[0], v2[1], v2[2]});
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return alpha_*alpha_ * domain_->get_bounding_sphere_squared_radius();
  }

  std::vector<std::vector<std::array<double, 3>>>
  stretch_features(
      const std::vector<std::vector<std::array<double, 3>>> & features
      ) const
  {
    std::vector<std::vector<std::array<double, 3>>> stretched_features;
    for (const auto & feature: features) {
      std::vector<std::array<double, 3>> stretched_feature;
      for (const auto & point: feature) {
        // scale the component of normalized_direction_ by alpha_
        const Eigen::Vector3d v(point.data());
        const double beta = normalized_direction_.dot(v);
        const auto v2 = beta * alpha_ * normalized_direction_
           + (v - beta * normalized_direction_);
        stretched_feature.push_back({v2[0], v2[1], v2[2]});
      }
      stretched_features.push_back(stretched_feature);
    }
    return stretched_features;
  }

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    return stretched_features_;
  };

  private:
    std::shared_ptr<const pygalmesh::DomainBase> domain_;
    const Eigen::Vector3d normalized_direction_;
    const double alpha_;
    const std::vector<std::vector<std::array<double, 3>>> stretched_features_;
};

class Intersection: public pygalmesh::DomainBase
{
  public:
  explicit Intersection(
      std::vector<std::shared_ptr<const pygalmesh::DomainBase>> & domains
      ):
    domains_(domains)
  {
  }

  virtual ~Intersection() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const
  {
    // TODO find a differentiable expression
    double maxval = std::numeric_limits<double>::lowest();
    for (const auto & domain: domains_) {
      maxval = std::max(maxval, domain->eval(x));
    }
    return maxval;
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

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    std::vector<std::vector<std::array<double, 3>>> features;
    for (const auto & domain: domains_) {
      const auto f = domain->get_features();
      features.insert(std::end(features), std::begin(f), std::end(f));
    }
    return features;
  };

  private:
    std::vector<std::shared_ptr<const pygalmesh::DomainBase>> domains_;
};

class Union: public pygalmesh::DomainBase
{
  public:
  explicit Union(
      std::vector<std::shared_ptr<const pygalmesh::DomainBase>> & domains
      ):
    domains_(domains)
  {
  }

  virtual ~Union() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const
  {
    // TODO find a differentiable expression
    double minval = std::numeric_limits<double>::max();
    for (const auto & domain: domains_) {
      minval = std::min(minval, domain->eval(x));
    }
    return minval;
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

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    std::vector<std::vector<std::array<double, 3>>> features;
    for (const auto & domain: domains_) {
      const auto f = domain->get_features();
      features.insert(std::end(features), std::begin(f), std::end(f));
    }
    return features;
  };

  private:
    std::vector<std::shared_ptr<const pygalmesh::DomainBase>> domains_;
};

class Difference: public pygalmesh::DomainBase
{
  public:
  Difference(
      std::shared_ptr<const pygalmesh::DomainBase> & domain0,
      std::shared_ptr<const pygalmesh::DomainBase> & domain1
      ):
    domain0_(domain0),
    domain1_(domain1)
  {
  }

  virtual ~Difference() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const
  {
    // TODO find a continuous (perhaps even differentiable) expression
    const double val0 = domain0_->eval(x);
    const double val1 = domain1_->eval(x);
    return (val0 < 0.0 && val1 >= 0.0) ? val0 : std::max(val0, -val1);
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return domain0_->get_bounding_sphere_squared_radius();
  }

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    std::vector<std::vector<std::array<double, 3>>> features;

    const auto f0 = domain0_->get_features();
    features.insert(std::end(features), std::begin(f0), std::end(f0));

    const auto f1 = domain1_->get_features();
    features.insert(std::end(features), std::begin(f1), std::end(f1));

    return features;
  };

  private:
    std::shared_ptr<const pygalmesh::DomainBase> domain0_;
    std::shared_ptr<const pygalmesh::DomainBase> domain1_;
};

} // namespace pygalmesh
#endif // DOMAIN_HPP
