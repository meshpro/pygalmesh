#include <memory>

class DomainBase {
  public:
    virtual double eval(
        const double x,
        const double y,
        const double z
        ) const = 0;
};

class Sphere: public DomainBase
{
  public:
    Sphere(
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

    virtual double eval(
        const double x,
        const double y,
        const double z
        ) const
    {
      const double xx0 = x - x0_;
      const double yy0 = y - y0_;
      const double zz0 = z - z0_;
      return xx0*xx0 + yy0*yy0 + zz0*zz0 - radius_*radius_;
    }

  private:
    const double x0_;
    const double y0_;
    const double z0_;
    const double radius_;
};


void generate_mesh(
    const std::shared_ptr<DomainBase> & in,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true
    );
