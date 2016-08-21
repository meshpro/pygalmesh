#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <memory>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

class DomainBase
{
  public:
  virtual K::FT operator()(K::Point_3 p) const = 0;
};

class Ball: public DomainBase
{
  public:
    // argument are double (not K::FT) for Python compatibility
    Ball(
        const double x0,
        const double y0,
        const double z0,
        const double radius
        ):
      x0_(x0),
      y0_(y0),
      z0_(z0),
      radius_(radius)
    {
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      const K::FT xx0 = p.x() - x0_;
      const K::FT yy0 = p.y() - y0_;
      const K::FT zz0 = p.z() - z0_;
      return xx0*xx0 + yy0*yy0 + zz0*zz0 - radius_*radius_;
    }

  private:
    const double x0_;
    const double y0_;
    const double z0_;
    const double radius_;
};


class Cuboid: public DomainBase
{
  public:
    Cuboid(
        const double x0,
        const double y0,
        const double z0,
        const double x1,
        const double y1,
        const double z1
        ):
      x0_(x0),
      y0_(y0),
      z0_(z0),
      x1_(x1),
      y1_(y1),
      z1_(z1)
    {
    }

    virtual K::FT operator()(K::Point_3 p) const
    {
      return (
          x0_ < p.x() && p.x() < x1_ &&
          y0_ < p.y() && p.y() < y1_ &&
          z0_ < p.z() && p.z() < z1_
          ) ? -1.0 : 1.0;
    }

  private:
    const double x0_;
    const double y0_;
    const double z0_;
    const double x1_;
    const double y1_;
    const double z1_;
};


void generate_mesh(
    const std::shared_ptr<DomainBase> & in,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true
    );
