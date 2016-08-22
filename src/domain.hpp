#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include "primitives.hpp"

#include <Eigen/Dense>

namespace loom {

class RotatedPrimitive: public loom::PrimitiveBase
{
  public:
  RotatedPrimitive(
      const std::shared_ptr<const loom::PrimitiveBase> & primitive,
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
    const std::shared_ptr<const loom::PrimitiveBase> primitive_;
    const Eigen::Vector3d normalized_axis_;
    const double sinAngle_;
    const double cosAngle_;
};

class Rotate: public loom::DomainBase
{
  public:
  Rotate(
      std::shared_ptr<const loom::DomainBase> & domain,
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

class Intersection: public loom::DomainBase
{
  public:
  Intersection(
      std::shared_ptr<const loom::DomainBase> & domain0,
      std::shared_ptr<const loom::DomainBase> & domain1
      ):
    primitives_(merge(domain0->get_primitives(), domain1->get_primitives())),
    signs_(intersection_signs(domain0->get_signs(), domain1->get_signs()))
  {
  }

  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  merge(
      const std::vector<std::shared_ptr<const loom::PrimitiveBase>> & a,
      const std::vector<std::shared_ptr<const loom::PrimitiveBase>> & b
    )
  {
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> out = a;
    out.insert(out.end(), b.begin(), b.end());
    return out;
  }

  std::vector<std::vector<CGAL::Sign>>
  intersection_signs(
      const std::vector<std::vector<CGAL::Sign>> & a,
      const std::vector<std::vector<CGAL::Sign>> & b
    )
  {
    std::vector<std::vector<CGAL::Sign>> out = {};
    for (size_t i=0; i < a.size(); i++) {
      for (size_t j=0; j < b.size(); j++) {
        auto c = a[i];
        c.insert(c.end(), b[j].begin(), b[j].end());
        out.push_back(c);
      }
    }
    return out;
  }

  virtual
  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  get_primitives() const
  {
    return primitives_;
  }

  virtual
  std::vector<std::vector<CGAL::Sign>>
  get_signs() const
  {
    return signs_;
  }

  private:
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> primitives_;
    std::vector<std::vector<CGAL::Sign>> signs_;
};

class Union: public loom::DomainBase
{
  public:
  Union(
      std::shared_ptr<const loom::DomainBase> & domain0,
      std::shared_ptr<const loom::DomainBase> & domain1
      ):
    primitives_(merge(domain0->get_primitives(), domain1->get_primitives())),
    signs_(union_signs(domain0->get_signs(), domain1->get_signs()))
  {
  }

  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  merge(
      const std::vector<std::shared_ptr<const loom::PrimitiveBase>> & a,
      const std::vector<std::shared_ptr<const loom::PrimitiveBase>> & b
    )
  {
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> out = a;
    out.insert(out.end(), b.begin(), b.end());
    return out;
  }

  std::vector<std::vector<CGAL::Sign>>
  append_plus_minus(const std::vector<std::vector<CGAL::Sign>> & in)
  {
    auto out = in;
    out.insert(out.end(), in.begin(), in.end());
    for (size_t i=0; i < in.size(); i++) {
      out[i].push_back(CGAL::POSITIVE);
      out[i+in.size()].push_back(CGAL::NEGATIVE);
    }
    return out;
  }

  std::vector<std::vector<CGAL::Sign>>
  union_signs(
      const std::vector<std::vector<CGAL::Sign>> & a,
      const std::vector<std::vector<CGAL::Sign>> & b
    )
  {
    std::vector<std::vector<CGAL::Sign>> out = {};

    // create all combinations of +- in b
    std::vector<std::vector<CGAL::Sign>> b_combinations = {{}};
    for (size_t i=0; i < b.size(); i++) {
      b_combinations = append_plus_minus(b_combinations);
    }

    // a fixed, all combinations in b
    for (size_t i=0; i < a.size(); i++) {
      for (size_t j=0; j < b_combinations.size(); j++) {
        auto c = a[i];
        c.insert(c.end(), b_combinations[j].begin(), b_combinations[j].end());
        out.push_back(c);
      }
    }

    // create all combinations of +- in a
    std::vector<std::vector<CGAL::Sign>> a_combinations = {{}};
    for (size_t i=0; i < a.size(); i++) {
      a_combinations = append_plus_minus(a_combinations);
    }

    // all combinations in a, b fixed
    for (size_t i=0; i < a_combinations.size(); i++) {
      for (size_t j=0; j < b.size(); j++) {
        auto c = a_combinations[i];
        c.insert(c.end(), b[j].begin(), b[j].end());
        out.push_back(c);
      }
    }

    return out;
  }

  virtual
  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  get_primitives() const
  {
    return primitives_;
  }

  virtual
  std::vector<std::vector<CGAL::Sign>>
  get_signs() const
  {
    return signs_;
  }

  private:
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> primitives_;
    std::vector<std::vector<CGAL::Sign>> signs_;
};

class Difference: public loom::DomainBase
{
  public:
  Difference(
      std::shared_ptr<const loom::DomainBase> & domain0,
      std::shared_ptr<const loom::DomainBase> & domain1
      ):
    primitives_(merge(domain0->get_primitives(), domain1->get_primitives())),
    signs_(difference_signs(domain0->get_signs(), domain1->get_signs()))
  {
  }

  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  merge(
      const std::vector<std::shared_ptr<const loom::PrimitiveBase>> & a,
      const std::vector<std::shared_ptr<const loom::PrimitiveBase>> & b
    )
  {
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> out = a;
    out.insert(out.end(), b.begin(), b.end());
    return out;
  }

  std::vector<std::vector<CGAL::Sign>>
  difference_signs(
      const std::vector<std::vector<CGAL::Sign>> & a,
      const std::vector<std::vector<CGAL::Sign>> & b
    )
  {
    // difference = intersection with the complement
    auto b_inv = b;
    for (size_t i=0; i < b_inv.size(); i++) {
      for (size_t j=0; j < b_inv[i].size(); j++) {
        b_inv[i][j] = (b[i][j] == CGAL::NEGATIVE) ?
          CGAL::POSITIVE :
          CGAL::NEGATIVE;
      }
    }

    std::vector<std::vector<CGAL::Sign>> out = {};
    for (size_t i=0; i < a.size(); i++) {
      for (size_t j=0; j < b_inv.size(); j++) {
        auto c = a[i];
        c.insert(c.end(), b_inv[j].begin(), b_inv[j].end());
        out.push_back(c);
      }
    }
    return out;
  }

  virtual
  std::vector<std::shared_ptr<const loom::PrimitiveBase>>
  get_primitives() const
  {
    return primitives_;
  }

  virtual
  std::vector<std::vector<CGAL::Sign>>
  get_signs() const
  {
    return signs_;
  }

  private:
    std::vector<std::shared_ptr<const loom::PrimitiveBase>> primitives_;
    std::vector<std::vector<CGAL::Sign>> signs_;
};

} // namespace loom
#endif // DOMAIN_HPP
