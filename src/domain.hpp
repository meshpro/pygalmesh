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
  virtual ~DomainBase();
  virtual double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  virtual std::vector<std::vector<std::array<double, 3>>> get_features();
};

class Translate: public pygalmesh::DomainBase
{
  public:
  Translate(
      const std::shared_ptr<const pygalmesh::DomainBase> & domain,
      const std::array<double, 3> & direction
            );
  virtual ~Translate();
  std::vector<std::vector<std::array<double, 3>>>
  translate_features(
      const std::vector<std::vector<std::array<double, 3>>> & features,
      const Eigen::Vector3d & direction
  );
  double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  virtual std::vector<std::vector<std::array<double, 3>>> get_features();

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
  );
  virtual ~Rotate();
  Eigen::Vector3d rotate(
      const Eigen::Vector3d & vec,
      const Eigen::Vector3d & axis,
      const double sinAngle,
      const double cosAngle
  );
  virtual double eval(const std::array<double, 3> & x);
  std::vector<std::vector<std::array<double, 3>>>
  rotate_features(
      const std::vector<std::vector<std::array<double, 3>>> & features
  );
  virtual double get_bounding_sphere_squared_radius();
  virtual std::vector<std::vector<std::array<double, 3>>>
  get_features();

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
        );
  virtual ~Scale();
  virtual double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  std::vector<std::vector<std::array<double, 3>>>
  scale_features(
      const std::vector<std::vector<std::array<double, 3>>> & features
                 );
  virtual std::vector<std::vector<std::array<double, 3>>> get_features();

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
          );

  virtual ~Stretch();

  virtual double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  std::vector<std::vector<std::array<double, 3>>>
  stretch_features(
      const std::vector<std::vector<std::array<double, 3>>> & features
                   );
  virtual std::vector<std::vector<std::array<double, 3>>>
  get_features();

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
                        );
  virtual ~Intersection();
  virtual double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  virtual std::vector<std::vector<std::array<double, 3>>>
  get_features();

  private:
    std::vector<std::shared_ptr<const pygalmesh::DomainBase>> domains_;
};

class Union: public pygalmesh::DomainBase
{
  public:
  explicit Union(
      std::vector<std::shared_ptr<const pygalmesh::DomainBase>> & domains
                 );
  virtual ~Union();
  virtual double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  virtual std::vector<std::vector<std::array<double, 3>>>
  get_features();

  private:
    std::vector<std::shared_ptr<const pygalmesh::DomainBase>> domains_;
};

class Difference: public pygalmesh::DomainBase
{
  public:
  Difference(
      std::shared_ptr<const pygalmesh::DomainBase> & domain0,
      std::shared_ptr<const pygalmesh::DomainBase> & domain1
             );
  virtual ~Difference();
  virtual double eval(const std::array<double, 3> & x);
  virtual double get_bounding_sphere_squared_radius();
  virtual std::vector<std::vector<std::array<double, 3>>>
  get_features();

  private:
    std::shared_ptr<const pygalmesh::DomainBase> domain0_;
    std::shared_ptr<const pygalmesh::DomainBase> domain1_;
};

} // namespace pygalmesh
#endif // DOMAIN_HPP
