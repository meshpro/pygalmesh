#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <memory>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

namespace loom {

// forward declarations
class DomainBase;
class PrimitiveBase;

class DomainBase
{
  public:
  virtual std::vector<std::shared_ptr<const loom::PrimitiveBase>> get_primitives() const = 0;
  virtual std::vector<std::vector<CGAL::Sign>> get_signs() const = 0;
};

class PrimitiveBase:
  public DomainBase,
  public std::enable_shared_from_this<PrimitiveBase>
{
  public:
  virtual K::FT operator()(K::Point_3 p) const = 0;

  virtual std::vector<std::shared_ptr<const loom::PrimitiveBase>> get_primitives() const
  {
    return {shared_from_this()};
  }
};

class Ball: public PrimitiveBase
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

    virtual std::vector<std::vector<CGAL::Sign>> get_signs() const
    {
      return {{CGAL::NEGATIVE}};
    }

  private:
    const std::vector<double> x0_;
    const double radius_;
};


class Cuboid: public PrimitiveBase
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

    virtual std::vector<std::vector<CGAL::Sign>> get_signs() const
    {
      return {{CGAL::NEGATIVE}};
    }

  private:
    const std::vector<double> x0_;
    const std::vector<double> x1_;
};


class Ellipsoid: public PrimitiveBase
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

    virtual std::vector<std::vector<CGAL::Sign>> get_signs() const
    {
      return {{CGAL::NEGATIVE}};
    }

  private:
    const std::vector<double> x0_;
    const double a0_2_;
    const double a1_2_;
    const double a2_2_;
};


class Cylinder: public PrimitiveBase
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
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      const K::FT xx0 = p.x();
      const K::FT yy0 = p.y();
      return (z0_ < p.z() && p.z() < z1_) ?
        p.x()*p.x() + p.y()*p.y() - radius2_ :
        1.0;
    }

    virtual std::vector<std::vector<CGAL::Sign>> get_signs() const
    {
      return {{CGAL::NEGATIVE}};
    }

  private:
    const double z0_;
    const double z1_;
    const double radius2_;
};

} // namespace loom

#endif // PRIMITIVES_HPP
