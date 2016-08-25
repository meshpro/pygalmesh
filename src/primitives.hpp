#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include "domain.hpp"

#include <memory>
#include <vector>

namespace loom {

class Ball: public loom::DomainBase
{
  public:
    // argument are double (not K::FT) for Python compatibility
    Ball(
        const std::vector<double> & x0,
        const double radius
        ):
      x0_(x0),
      radius_(radius)
    {
      assert(x0_.size() == 3);
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      const K::FT xx0 = p.x() - x0_[0];
      const K::FT yy0 = p.y() - x0_[1];
      const K::FT zz0 = p.z() - x0_[2];
      return xx0*xx0 + yy0*yy0 + zz0*zz0 - radius_*radius_;
    }

    virtual
    double
    get_bounding_sphere_squared_radius() const
    {
      const double x0_nrm = sqrt(x0_[0]*x0_[0] + x0_[1]*x0_[1] + x0_[2]*x0_[2]);
      return (x0_nrm + radius_) * (x0_nrm + radius_);
    }

  private:
    const std::vector<double> x0_;
    const double radius_;
};


class Cuboid: public loom::DomainBase
{
  public:
    Cuboid(
        const std::vector<double> & x0,
        const std::vector<double> & x1
        ):
      x0_(x0),
      x1_(x1)
    {
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      return (
          x0_[0] < p.x() && p.x() < x1_[0] &&
          x0_[1] < p.y() && p.y() < x1_[1] &&
          x0_[2] < p.z() && p.z() < x1_[2]
          ) ? -1.0 : 1.0;
    }

    virtual
    double
    get_bounding_sphere_squared_radius() const
    {
      const double x0_nrm2 = x0_[0]*x0_[0] + x0_[1]*x0_[1] + x0_[2]*x0_[2];
      const double x1_nrm2 = x1_[0]*x1_[0] + x1_[1]*x1_[1] + x1_[2]*x1_[2];
      return std::max({x0_nrm2, x1_nrm2});
    }

    virtual
    std::list<std::vector<K::Point_3>>
    get_features() const
    {
      std::vector<K::Point_3> corners = {
        K::Point_3(x0_[0], x0_[1], x0_[2]),
        K::Point_3(x1_[0], x0_[1], x0_[2]),
        K::Point_3(x0_[0], x1_[1], x0_[2]),
        K::Point_3(x0_[0], x0_[1], x1_[2]),
        K::Point_3(x1_[0], x1_[1], x0_[2]),
        K::Point_3(x1_[0], x0_[1], x1_[2]),
        K::Point_3(x0_[0], x1_[1], x1_[2]),
        K::Point_3(x1_[0], x1_[1], x1_[2])
      };
      return {
          {corners[0], corners[1]},
          {corners[0], corners[2]},
          {corners[0], corners[3]},
          {corners[1], corners[4]},
          {corners[1], corners[5]},
          {corners[2], corners[4]},
          {corners[2], corners[6]},
          {corners[3], corners[5]},
          {corners[3], corners[6]},
          {corners[4], corners[7]},
          {corners[5], corners[7]},
          {corners[6], corners[7]}
        };
    };

  private:
    const std::vector<double> x0_;
    const std::vector<double> x1_;
};


class Ellipsoid: public loom::DomainBase
{
  public:
    Ellipsoid(
        const std::vector<double> & x0,
        const double a0,
        const double a1,
        const double a2
        ):
      x0_(x0),
      a0_2_(a0*a0),
      a1_2_(a1*a1),
      a2_2_(a2*a1)
    {
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      const K::FT xx0 = p.x() - x0_[0];
      const K::FT yy0 = p.y() - x0_[1];
      const K::FT zz0 = p.z() - x0_[2];
      return xx0*xx0/a0_2_ + yy0*yy0/a1_2_ + zz0*zz0/a2_2_ - 1.0;
    }

    virtual
    double
    get_bounding_sphere_squared_radius() const
    {
      return std::max({a0_2_, a1_2_, a2_2_});
    }

  private:
    const std::vector<double> x0_;
    const double a0_2_;
    const double a1_2_;
    const double a2_2_;
};


class Cylinder: public loom::DomainBase
{
  public:
    Cylinder(
        const double z0,
        const double z1,
        const double radius
        ):
      z0_(z0),
      z1_(z1),
      radius2_(radius*radius)
    {
      assert(z1_ > z0_);
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      const K::FT xx0 = p.x();
      const K::FT yy0 = p.y();
      return (z0_ < p.z() && p.z() < z1_) ?
        p.x()*p.x() + p.y()*p.y() - radius2_ :
        1.0;
    }

    virtual
    double
    get_bounding_sphere_squared_radius() const
    {
      return (z1_ - z0_)*(z1_ - z0_) + radius2_;
    }

  private:
    const double z0_;
    const double z1_;
    const double radius2_;
};

} // namespace loom

#endif // PRIMITIVES_HPP
